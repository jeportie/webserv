#!/usr/bin/env python3
import os
import sys
# khsdfgghfshdg
print("Content-Type: text/html")
print()
print("<h1>Hello from CGI!</h1>")

# Récupérer la longueur du body (POST)
content_length = os.environ.get("CONTENT_LENGTH")
if content_length and content_length.isdigit() and int(content_length) > 0:
    body = sys.stdin.read(int(content_length))
    print("<pre>")
    print(body)
    print("</pre>")

# while True:
#     pass  # boucle infinie pour tester le timeout du serveur