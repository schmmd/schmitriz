/* Schmitriz service worker — makes the game fully playable offline.
 *
 * The game is self-contained (index.html + schmitriz.js + assets.js, with all
 * art embedded as data URIs), so caching the app shell is enough to run with no
 * network. Strategy: precache the shell on install; serve same-origin GETs
 * stale-while-revalidate (instant offline, refreshed in the background when
 * online). Cross-origin requests (the Supabase leaderboard) are left to the
 * network and already fall back to local scores when offline.
 *
 * Bump CACHE when the shell changes so old caches are cleaned up on activate.
 */
const CACHE = 'schmitriz-v1';
const SHELL = [
  './',
  './index.html',
  './schmitriz.js',
  './assets.js',
  './manifest.webmanifest',
  './icon-192.png',
  './icon-512.png',
];

self.addEventListener('install', (e) => {
  e.waitUntil(
    caches.open(CACHE).then((c) => c.addAll(SHELL)).then(() => self.skipWaiting())
  );
});

self.addEventListener('activate', (e) => {
  e.waitUntil(
    caches.keys()
      .then((keys) => Promise.all(keys.filter((k) => k !== CACHE).map((k) => caches.delete(k))))
      .then(() => self.clients.claim())
  );
});

self.addEventListener('fetch', (e) => {
  const req = e.request;
  if (req.method !== 'GET') return;
  const url = new URL(req.url);
  if (url.origin !== self.location.origin) return;   // let Supabase etc. hit the network

  e.respondWith((async () => {
    const cache = await caches.open(CACHE);
    const cached = await cache.match(req, { ignoreSearch: true });
    const network = fetch(req)
      .then((res) => { if (res && res.ok) cache.put(req, res.clone()); return res; })
      .catch(() => null);
    // stale-while-revalidate; fall back to the app shell for offline navigations
    return cached || (await network) || cache.match('./index.html');
  })());
});
