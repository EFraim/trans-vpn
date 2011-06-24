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

def _add_text_to_node(node, text):
    text_node = minidom.Text()
    text_node.data = text
    node.appendChild(text_node)

def _create_key_node(key_name, key_value):
    key_node = minidom.Element(key_name)
    _add_text_to_node(key_node, "\n")
    _add_text_to_node(key_node, key_value.encode('base64'))
    return key_node
    

def _create_rsaid_node(rsaid):
    rsaid_node = minidom.Element("rsa_id")
    if rsaid.public_key:
        rsaid_node.appendChild(_create_key_node("public_key", rsaid.public_modulus))
    if rsaid.private_key:
        rsaid_node.appendChild(_create_key_node("private_key", rsaid.private_key))
    return rsaid_node

def _create_entities_node(rsaid_nodes):
    entities_node = minidom.Element("entities")
    for rsaid_node in rsaid_nodes:
        entities_node.appendChild(rsaid_node)
    return entities_node

def to_file(entities, filename):
    if isinstance(entities, list):
        entity_nodes = [_create_rsaid_node(entity) for entity in entities]
        root_node = _create_entities_node(entity_nodes)
    elif isinstance(entities, RSAId):
        root_node = _create_rsaid_node(entities)
    else:
        raise RuntimeError("Invalid type of 'entities' argument")

    root_node.writexml(file(filename, 'w'))
    