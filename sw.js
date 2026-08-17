/* Schmitriz service worker — makes the game fully playable offline.
 *
 * The game is self-contained (index.html + schmitriz.js + assets.js, with all
 * art embedded as data URIs), so precaching the shell is enough to run with no
 * network. Cross-origin requests (the Supabase leaderboard) are left to the
 * network and already fall back to local scores when offline.
 *
 * Requests are served stale-while-revalidate: the cache answers immediately
 * (fast, and works offline) while the network refreshes it in the background.
 * The catch with that strategy on its own is that a freshly deployed build only
 * lands in the cache during one visit and is not *used* until the next one, so
 * players sit a version behind with no way to know. To close that gap the
 * revalidation compares the new response's validator against the cached one and
 * posts an `update-ready` message, which the page turns into a reload prompt.
 *
 * CACHE is the cache namespace; bumping it discards earlier caches on activate.
 */
const CACHE = 'schmitriz-v2';
const SHELL = [
  './',
  './index.html',
  './schmitriz.js',
  './assets.js',
  './manifest.webmanifest',
  './icon-192.png',
  './icon-512.png',
  './icon-maskable-192.png',
  './icon-maskable-512.png',
];

// Everything precached is part of the build, so a change to any of it means a
// new deploy has landed.
const SHELL_URLS = new Set(SHELL.map((u) => new URL(u, self.location).href));

self.addEventListener('install', (e) => {
  e.waitUntil((async () => {
    const cache = await caches.open(CACHE);
    await Promise.all(SHELL.map(async (u) => {
      try {
        // `reload` bypasses the browser's HTTP cache so a fresh install cannot
        // bake in a stale copy of the shell.
        await cache.add(new Request(u, { cache: 'reload' }));
      } catch (err) {
        // Fail the install rather than leave a half-cached shell, but name the
        // file — otherwise a renamed asset kills offline support silently.
        throw new Error('precache failed for ' + u + ': ' + err);
      }
    }));
    await self.skipWaiting();
  })());
});

self.addEventListener('activate', (e) => {
  e.waitUntil((async () => {
    const keys = await caches.keys();
    await Promise.all(keys.filter((k) => k !== CACHE).map((k) => caches.delete(k)));
    await self.clients.claim();
  })());
});

// Two responses are the same build if their validators match.
const revision = (res) => res && (res.headers.get('etag') || res.headers.get('last-modified'));

let announced = false;
async function announceUpdate() {
  if (announced) return;
  announced = true;
  for (const client of await self.clients.matchAll({ type: 'window' })) {
    client.postMessage({ type: 'update-ready' });
  }
}

self.addEventListener('fetch', (e) => {
  const req = e.request;
  if (req.method !== 'GET') return;
  const url = new URL(req.url);
  if (url.origin !== self.location.origin) return;   // let cross-origin hit the network

  e.respondWith((async () => {
    const cache = await caches.open(CACHE);
    const cached = await cache.match(req, { ignoreSearch: true });

    const network = fetch(req).then(async (res) => {
      // 200 only: a 206 range response (audio seeking) cannot be cached, and a
      // redirect or error page must not overwrite a good entry.
      if (!res || res.status !== 200) return res;
      // Store under the search-less URL, which is how the lookup above matches,
      // so query-string variants cannot pile up as copies nothing ever reads.
      const key = new URL(req.url);
      key.search = '';
      await cache.put(key.href, res.clone());
      if (cached && SHELL_URLS.has(key.href) && revision(cached) !== revision(res)) {
        await announceUpdate();
      }
      return res;
    }).catch(() => null);

    if (cached) {
      // The response is already settled, so the background revalidation has to
      // be kept alive explicitly or the worker can be killed before it writes.
      e.waitUntil(network);
      return cached;
    }
    const res = await network;
    if (res) return res;
    // Offline with nothing cached. Only a navigation can sensibly be answered
    // with the app shell; anything else would get HTML where it wanted an asset.
    if (req.mode === 'navigate') return cache.match('./index.html');
    return new Response('', { status: 504, statusText: 'offline' });
  })());
});
