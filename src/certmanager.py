#!/usr/bin/python3

from OpenSSL.crypto import PKey, TYPE_RSA, X509, X509Name, X509Extension
from OpenSSL.crypto import dump_certificate, dump_privatekey, FILETYPE_PEM, FILETYPE_PEM
from OpenSSL.crypto import load_certificate, load_privatekey
from secrets import randbelow
from sys import maxsize
from pathlib import Path

'''
Based on:
https://github.com/titeuf87/python3-tls-example/blob/master/certmanager.py
'''

def createRootCa():
    cert: X509 = X509()
    cert.set_version(3)
    cert.set_serial_number(randbelow(maxsize))
    cert.gmtime_adj_notBefore(0)
    cert.gmtime_adj_notAfter(60 * 60 * 24 * 365)

    subject: X509Name = cert.get_subject()
    subject.CN = "push.uwpx.org"
    subject.O = "UWPX Push Server"

    issuer: X509Name = cert.get_issuer()
    issuer.CN = "push.uwpx.org"
    issuer.O = "UWPX Push Server"

    pKey: PKey = PKey()
    pKey.generate_key(TYPE_RSA, 4096)
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
    cert.sign(pKey, "sha256")

    with open("cert/root.pem", "wb") as certFile:
        certFile.write(dump_certificate(FILETYPE_PEM, cert))

    with open("cert/root.key", "wb") as pKeyFile:
        pKeyFile.write(dump_privatekey(FILETYPE_PEM, pKey))

def createCertificate(subjectCn: str, subjectO: str, serverside: bool, certFilename: str, pKeyFilename: str):
    # Load the root cert:
    rootPem: str = ""
    with open("cert/root.pem", "rb") as rootPemFile:
        rootPem = rootPemFile.read()
    rootKey: str = ""
    with open("cert/root.key", "rb") as rootKeyFile:
        rootKey = rootKeyFile.read()

    ca_cert: X509 = load_certificate(FILETYPE_PEM, rootPem)
    ca_key: PKey = load_privatekey(FILETYPE_PEM, rootKey)

    # Genrate Certificate:
    cert: X509 = X509()
    cert.set_serial_number(randbelow(maxsize))
    cert.gmtime_adj_notBefore(0)
    cert.gmtime_adj_notAfter(60 * 60 * 24 * 365)
    cert.set_version(3)

    subject: X509Name = cert.get_subject()
    subject.CN = subjectCn
    subject.O = subjectO

    if serverside:
        cert.add_extensions([X509Extension(b"subjectAltName",
                                           False,
                                           b"DNS:test1.example.com,DNS:test2.example.com")])

    cert.set_issuer(ca_cert.get_subject())

    pKey: PKey = PKey()
    pKey.generate_key(TYPE_RSA, 4096)
    cert.set_pubkey(pKey)
    cert.sign(ca_key, "sha256")

    with open(certFilename, "wb") as certFile:
        certFile.write(dump_certificate(FILETYPE_PEM, cert))

    with open(pKeyFilename, "wb") as pKeyFile:
        pKeyFile.write(dump_privatekey(FILETYPE_PEM, pKey))

def createDirectory():
    Path("cert").mkdir(parents=True, exist_ok=True)

def main():
    print("Creating output directory...")
    createDirectory()
    print("Creating root CA...")
    createRootCa()
    print("Creating server certificate...")
    createCertificate("server", "UWPX Push Server", True, "cert/server.crt", "cert/server.key")
    print("Creating client certificate...")
    createCertificate("client", "UWPX Push Server", True, "cert/client.crt", "cert/client.key")
    print("Done.")


if __name__ == '__main__':
    main()