#!/usr/bin/python3

from OpenSSL.crypto import PKey, TYPE_RSA, X509, X509Name, X509Extension
from OpenSSL.crypto import dump_certificate, dump_privatekey, FILETYPE_PEM, FILETYPE_PEM
from secrets import randbelow
from sys import maxsize

'''
Based on:
https://github.com/titeuf87/python3-tls-example/blob/master/certmanager.py
'''

def createRootCa():
    pKey: PKey = PKey()
    pKey.generate_key(TYPE_RSA, 8192)

    cert: X509 = X509()
    cert.set_version(3)
    cert.set_serial_number(randbelow(maxsize))
    cert.gmtime_adj_notBefore(0)
    cert.gmtime_adj_notAfter(60 * 60 * 24 * 365)

    subject: X509Name = cert.get_subject()
    subject.CN = "example.com"
    subject.O = "mycommonname"

    issuer: X509Name = cert.get_issuer()
    issuer.CN = "example.com"
    issuer.O = "mycommonname"

    cert.set_pubkey(pKey)
    cert.add_extensions([
        X509Extension(b"basicConstraints",
                      True,
                      b"CA:TRUE"),
        X509Extension(b"subjectKeyIdentifier",
                      False,
                      b"hash",
                      subject=cert)
    ])
    cert.add_extensions([
        X509Extension(b"authorityKeyIdentifier",
                      False,
                      b"keyid:always",
                      issuer=cert)
    ])
    cert.sign(pKey, "sha1")

    with open("root.pem", "wb") as certFile:
        certFile.write(dump_certificate(FILETYPE_PEM, cert))

    with open("root.key", "wb") as pKeyFile:
        pKeyFile.write(dump_privatekey(FILETYPE_PEM, pKey))

def main():
    createRootCa()


if __name__ == '__main__':
    main()