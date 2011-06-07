import xml.dom.minidom as minidom

class RSAId:
    GLOBAL_PUBLIC_KEY = "\x00" * 125 + "\x01\x00\x01"
    
    def __init__(self, public_modulus, private_key = ""):
        self.public_modulus = public_modulus
        self.public_key = self.GLOBAL_PUBLIC_KEY
        self.private_key = private_key
        
    def __repr__(self):
        return "SecureId(\npublic_modulus=%s,\npublic_key=%s,\nprivate_key=%s\n)" %   \
                (self.public_modulus.encode('hex'),                             \
                 self.public_key.encode('hex'),                                 \
                 self.private_key.encode('hex'))

    def __hash__(self):
        return hash((self.public_modulus, self.public_key, self.private_key))

    def __eq__(self, other):
        return self.public_modulus == other.public_modulus and \
               self.public_key == other.public_key and \
               self.private_key == other.private_key


def _filter_element_nodes(nodes):
    return [node for node in nodes if node.nodeType == node.ELEMENT_NODE]

def _get_key_value(keyNode):
    return keyNode.firstChild.nodeValue.decode('base64')


def _parse_rsa_id(xmlNode):
    public_key = ""
    private_key = ""
    values = _filter_element_nodes(xmlNode.childNodes)
    for value in values:
        if value.nodeName == "public_key":
            public_key = _get_key_value(value)
        elif value.nodeName == "private_key":
            private_key = _get_key_value(value)
        else:
            raise RuntimeError("unkown tag: " + value.nodeName)
    
    return RSAId(public_key, private_key)


def from_file(filename):
    doc = minidom.parse(filename)
    root = doc.childNodes[0]
    if root.nodeName == "rsa_id":
        return _parse_rsa_id(root)
    elif root.nodeName == "entities":
        return [_parse_rsa_id(node) for node in _filter_element_nodes(root.childNodes)]
    else:
        raise RuntimeError("unkown root tag: " + root.nodeName)
    