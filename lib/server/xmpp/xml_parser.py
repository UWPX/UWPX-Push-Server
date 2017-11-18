from xml.dom import minidom


class XML_Parser(object):

	xml_dom = None

	def __init__(self, xml_str):
		self.xml_dom = minidom.parseString(xml_str)
		# use minidom.parse(filename) to process files

	def pushNotification(self):
		var_dict = dict()
		var_dict['wns_id'] = value

		for item in var_dict:
			if item is None or  item == "NULL":
				raise Exception('Error: Missing elememt in XML string!')
		return var_dict


if __name__ == '__main__':
    pass