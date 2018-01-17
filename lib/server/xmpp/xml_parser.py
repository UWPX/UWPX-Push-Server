from xml.dom import minidom


class XML_Parser(object):

    xml_dom = None

    def __init__(self, xml_str):
        self.xml_dom = minidom.parseString(xml_str)
        # self.xml_dom = minidom.parse('C:\\Users\\Tim\\Desktop\\test.xml')
        # use minidom.parse(filename) to process files

    def pushToTupel(self):
        # Search for tag 'push'
        push = self.xml_dom.getElementsByTagName('push')[0]
        var_dict = dict()
        # Write values of attributes to dictionary
        var_dict['client_id'] = push.attributes['clientId'].value
        var_dict['push_channel'] = push.attributes['pushChannel'].value

        # Throw exception if an item of the dictionary is None / NULL
        for item in var_dict:
            if item is None or item == "NULL":
                raise Exception('Error: Missing elememt in XML string!')

        return (var_dict['client_id'], var_dict['push_channel'])
        # tupel body: see clientmanager.py


if __name__ == '__main__':
    pass

    """
    teststr = "<push clientId='sauter.fabian@mailbox.org' pushChannel='https://db5.not ify.windows.com/?token=AwYAAACyXwnMijF375zd8XnwRkbdDuPhdn5ODXFjlTbcLCgqa%2fhH%2bU4kA3JSjlelYJEJ1d0tfqN0HsgIKkyQoBVD%2frK3eOwGSutgqmsyJXEmt9%2bg4O1RrjSH3%2fujvyxvujwLv6nJhOZjxMIDp2o4hjB0jIGt' />"
    parser = XML_Parser(teststr)
    push_dict = parser.pushToTupel()

    for x, y in push_dict.items():
        print(x + ": " + y)
    """
