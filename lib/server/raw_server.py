import socket
import ssl
from threading import Thread


class raw_Server(object):
    def __init__(self, hostname: str, thelog, port: int) -> None:
        self.__sock = socket.socket(socket.AF_INET)
        self.__hostname = hostname
        self.log = thelog
        self.__port = port
        self.__active = False

    def connect(self) -> None:
        try:
            self.__sock.bind(('', self.__port))
        except socket.error as e:
            self.log.printError("server bind on port: "+str(self.__port) + " failed.")
            exit(-1)
        self.log.printMessage("server sccesfuly binded port: "+str(self.__port))
        self.__sock.listen(10)
        self.__active = True

    def close(self) -> None:
        self.__active = False
        self.log.printMessage("closeing server, requested by other class")

    def __processClient(self, conn) -> None:
        raise ImplementationError("raw_Server isn't ment to be used as standalone")

    def upgradetoTLS(self) -> socket.socket:
        # if raw server is never client its possible to remove if
        if(True):
            try:
                self.__sock = ssl.wrap_socket(self.__sock, server_side=True, certfile='cert/tls.cert', keyfile='cert/tls.key', ssl_version=ssl.PROTOCOL_TLSv1_2)
            except (IOError, ssl.SSLError) as e:
                self.log.printError("cert or keyfile faulty: " + str(e))
                self.cleanup()
                exit(-1)
        else:
            self.__context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
            self.__context.verify_mode = ssl.CERT_REQUIRED
            try:
                self.__context.load_cert_chain(certfile='cert/tls.cert', keyfile='cert/tls.key')
            except (IOError, ssl.SSLError) as e:
                self.log.printError("cert or keyfile faulty: " + str(e))
                self.cleanup()
                exit(-1)
            self.__context.check_hostname = True
            self.__context.load_verify_locations("/etc/pki/tls/certs/ca-bundle.crt")
            self.__sock = self.__context.wrap_socket(self.__sock, server_hostname=self.__hostname)
        return self.__sock

    def cleanup(self)-> None:
        self.__sock.close()

    def loop(self)-> None:
        while self.__active:
            try:
                conn, addr = self.__sock.accept()
                Thread(target=self.__processClient, args=(conn,)).start()
            except ssl.SSLError:
                self.log.printWarning("unecrypted connection, blocked!")
        self.cleanup()


class ImplementationError(Exception):
    pass
