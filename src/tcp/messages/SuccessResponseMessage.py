from tcp.messages.AbstractResponseMessage import AbstractResponseMessage

class SuccessResponseMessage(AbstractResponseMessage):
    STATUS: int = 1

    def __init__(self):
        super(SuccessResponseMessage, self).__init__(self.STATUS)
