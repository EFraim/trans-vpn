import sys
import os
import xml.dom.minidom as minidom

import rsaid


def print_usage():
    usage_string = """Usage:
./keygenerator <action> [<arguments>]

./keygenerator -generate <filename>
    Generates key pair and stores it in the XML file
./keygenerator -print-in-hex <filename>
    Prints keys from the XML file in HEX mode
./keygenerator -remove-private <input-filename> <output-filename>
    Removes private key from the given input file and stores the result in output file
./keygenerator -add-entities <entities-file> [<rsaid-file> ... ]
    Appends public key of the several entities to a given entities file. If <entities-file> does not exist it will be created
"""
    print >> sys.stderr, usage_string
    return 1
    
def generate_keys(args):
    if len(args) != 1:
        return print_usage()
    
    filename = args[0]
    
    import polarssl
    rsa = polarssl.RSA()
    rsa.generate_keypair(1024)
    
    public_modulus = rsa.get_public_modulus().decode('hex')
    private_key = rsa.get_private_key().decode('hex')
    rsa_id = rsaid.RSAId(public_modulus, private_key)
    
    rsaid.to_file(rsa_id, filename)
    
    return 0
    

def print_keys_in_hex(args):
    if len(args) != 1:
        return print_usage()
    
    filename = args[0]

    rsa_id = rsaid.from_file(filename)
    
    if rsa_id.public_modulus:
        print "public_key:\n%s" % rsa_id.public_modulus.encode('hex')
    if rsa_id.private_key:
        print "private_key:\n%s" % rsa_id.private_key.encode('hex')
    
    return 0


def remove_private_key(args):
    if len(args) != 2:
        return print_usage()
    
    input_filename = args[0]
    output_filename = args[1]
    
    rsa_id = rsaid.from_file(input_filename)
    rsa_id.private_key = ""
    rsaid.to_file(rsa_id, output_filename)
    
    return 0

    
def add_entity(args):
    if len(args) < 1:
        return print_usage()
    
    filename = args[0]
    entities_filenames = args[1:]
    
    if os.path.exists(filename): 
        rsa_ids = rsaid.from_file(filename)
    else:
        rsa_ids = []
    
    for entity_filename in entities_filenames:
        rsa_id = rsaid.from_file(entity_filename)
        rsa_id.private_key = ""
        rsa_ids.append(rsa_id)
    
    rsaid.to_file(rsa_ids, filename)
    
    return 0


def main(argv):
    if len(argv) > 1:
        if argv[1] == "-generate":
            return generate_keys(argv[2:])
        elif argv[1] == "-print-in-hex":
            return print_keys_in_hex(argv[2:])
        elif argv[1] == "-remove-private":
            return remove_private_key(argv[2:])
        elif argv[1] == "-add-entities":
            return add_entity(argv[2:])
    
    return print_usage()
    

if __name__ == "__main__":
    sys.exit(main(sys.argv))
