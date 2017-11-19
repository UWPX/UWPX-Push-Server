from lib.log import log
from xml.dom import minidom


class xml_Processor(object):

    def __init__(self, log, serverAddr):
        self.log = log
        self.serverAddr = serverAddr

    def sessionEncrytable(xml):
        return True

    def identifyAsPush(self, userID, resource):
        pass

class xml_generator(object):
  
    hostname = None

    def __init__(self, hostname):
        self.hostname = hostname

    def createXML(self, list):



        # Sample Code
        XMLvalues = { 'fielda': "atesta", 'fieldb': "btestb"}
        XMLvalues2 = { 'fielda22': "atesta22", 'fieldb22': "btestb22"}
        doc = minidom.Document()
        root = doc.createElement("User")
        root.setAttribute("id", 'myId')
        root.setAttribute("email", 'zefix@mail.com')
        root.setAttribute("wns", "safajflafekfskf3593fesfsdf")
        doc.appendChild(root)
        for value in XMLvalues:
           # Create Element
           tempChild = doc.createElement(value)
           tempChild.setAttribute("var", "11")
           root.appendChild(tempChild)
        
           # Write Text
           nodeText = doc.createTextNode(XMLvalues[value].strip())
           tempChild.appendChild(nodeText)

           for value2 in XMLvalues2:
                tempChild2 = doc.createElement(value2)
                tempChild2.setAttribute("var2", "2222")
                tempChild.appendChild(tempChild2)

        print(doc.toprettyxml())
