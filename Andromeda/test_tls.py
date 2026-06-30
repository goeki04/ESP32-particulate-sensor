import urllib.request, urllib.error, ssl, os

URL   = "https://pi.andromeda-cloud.net/api/"
PROXY = "http://127.0.0.1:3128"

print("Python/OpenSSL:", ssl.OPENSSL_VERSION)
print("env HTTPS_PROXY:", os.environ.get("HTTPS_PROXY"))

ctx = ssl._create_unverified_context()  # spiegelt verify_none
opener = urllib.request.build_opener(
    urllib.request.ProxyHandler({"http": PROXY, "https": PROXY}),
    urllib.request.HTTPSHandler(context=ctx),
)
try:
    r = opener.open(URL, timeout=15)
    print("OK durch Proxy ->", r.status)
except urllib.error.HTTPError as e:
    print("Server erreicht (durch Proxy) -> HTTP", e.code)  # 401 = Erfolg
except Exception as e:
    print("FEHLER:", type(e).__name__, e)
