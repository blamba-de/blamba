# blamba

Apache2 config:
```
SetEnv ap_trust_cgilike_cl 1
```
Content-Length header is being set by PHP. Apache2 usually doesn't trust Content-Length headers, which then results in a missing progress indication on file downloads.