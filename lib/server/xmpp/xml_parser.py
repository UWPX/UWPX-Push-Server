from xml.dom import minidom


class XML_Parser(object):

	xml_dom = None

	def __init__(self, xml_str):
		self.xml_dom = minidom.parseString("<push clientId='sauter.fabian@zefix.xyz' chatId='kili@zefix.xyzsauter.fabian@zefix.xyz' key='someKey' wns_id='123' wns_secret='string'/>")
		# use minidom.parse(filename) to process files

	def pushNotification(self):
		# Search for tag 'push'
		push = self.xml_dom.getElementsByTagName("push")[0]

		var_dict = dict()
		# Write values of attributes to dictionary
		var_dict['wns_id'] = push.attributes['wns_id'].value
		var_dict['client_id'] = push.attributes['clientId'].value
		var_dict['chat_id'] = push.attributes['chatId'].value
		var_dict['wns_secret'] = push.attributes['wns_secret'].value

		# Throw exception if an item of the dictionary is None / NULL
		for item in var_dict:
			if item is None or item == "NULL":
				raise Exception('Error: Missing elememt in XML string!')
		return var_dict


if __name__ == '__main__':
	pass

	"""
	teststr = "<push clientId='sauter.fabian@zefix.xyz' chatId='kili@zefix.xyzsauter.fabian@zefix.xyz' key='someKey' wns_id=123 wns_secret='string'/>"
	parser = XML_Parser(teststr)
	push_dict = parser.pushNotification()
	for x, y in push_dict.items():
		print(x + ": " + y)
	"""