import ctypes
import struct

lib = ctypes.CDLL("libpolarssl.so")

class RSA:
    """
    Implements basic RSA functionality
    """
    RSA_PKCS_V15 = 0
    
    PUBLIC = 0
    PRIVATE = 1
    
    def __init__(self):
        self._ctx = lib.alloc_rsa_context()
        lib.rsa_init(self._ctx, RSA.RSA_PKCS_V15, 0)

    def __del__(self):
        lib.free_rsa_context(self._ctx)

    
    def generate_keypair(self, nbits):
        """
        Generates pair of public/private keys of size 'nbits' bits.
        """
        ret = lib.rsa_gen_keypair(self._ctx, nbits)
        if ret != 0:
            raise RuntimeError("Key generation failed: %d" % ret)
        self._keylen = (nbits + 7) // 8
        self._keyhexlen = self._keylen * 2 + 3


    def _pass_string(self, func, string):
        buf = ctypes.create_string_buffer(string)
        func(self._ctx, buf)
        
    def _get_string(self, func, maxlen):
        stringbuf = ctypes.create_string_buffer("\x00", maxlen)
        stringlen = ctypes.c_int(maxlen)
        func(self._ctx, stringbuf, ctypes.addressof(stringlen))
        return stringbuf.value


    def get_public_modulus(self):
        """
        Returns public modulus encoded in hex.
        """
        return self._get_string(lib.rsa_get_public_modulus, self._keyhexlen)
    
    def get_public_key(self):
        """
        Returns public key encoded in hex.
        """
        return self._get_string(lib.rsa_get_public_key, self._keyhexlen)

    def get_private_key(self):
        """
        Returns private key encoded in hex.
        """
        return self._get_string(lib.rsa_get_private_key, self._keyhexlen)


    def set_public_modulus(self, public_modulus):
        """
        Sets public modulus (input is encoded in hex).
        """
        self._keylen = len(public_modulus) // 2
        self._keyhexlen = self._keylen * 2 + 3
        self._pass_string(lib.rsa_set_public_modulus, public_modulus)
        
    def set_public_key(self, public_key):
        """
        Sets public key (input is encoded in hex).
        """
        self._pass_string(lib.rsa_set_public_key, public_key)

    def set_private_key(self, private_key):
        """
        Sets private key (input is encoded in hex).
        """
        self._pass_string(lib.rsa_set_private_key, private_key)


    def encrypt(self, data, mode = PUBLIC):
        """
        Performs public key operation.
        Input data should be string of the same length as public modulus (N),
        but smaller that it alphabetically.
        In case provided input is shorter, it's padded with zeros from the left.
        (thus it's guaranteed to be smaller that N)
        
        Returned result is a string of size of public modulus.
        """
        output_size = ctypes.c_int(lib.rsa_get_encryption_size(self._ctx, len(data)))
        output = ctypes.create_string_buffer("\x00", output_size.value)
        input = ctypes.create_string_buffer(data, len(data))
        if lib.rsa_encrypt(self._ctx, input, len(input), output, ctypes.byref(output_size), mode) != 0:
            raise RuntimeError("Error in rsa_encrypt")
        return output.raw[:output_size.value]
    
    def decrypt(self, data, mode = PRIVATE):
        """
        Performs public key operation.
        Input data should be string of the same length as public modulus (N),
        but smaller that it alphabetically.
        In case provided input is shorter, it's padded with zeros from the left.
        (thus it's guaranteed to be smaller that N)
        
        Returned result is a string of size of public modulus.
        """
        output_size = ctypes.c_int(lib.rsa_get_decryption_size(self._ctx, len(data)))
        output = ctypes.create_string_buffer("\x00", output_size.value)
        input = ctypes.create_string_buffer(data, len(data))
        if lib.rsa_decrypt(self._ctx, input, len(input), output, ctypes.byref(output_size), mode) != 0:
            raise RuntimeError("Error in rsa_encrypt")
        return output.raw[:output_size.value]


class AES:
    """
    Implements AES encrytion.
    """
    MODE_ECB = 0
    MODE_CBC = 1
    
    OP_ENCRYPT = 1
    OP_DECRYPT = 0
    
    def __init__(self):
        self._ctx = lib.alloc_aes_context()
    
    def __del__(self):
        lib.free_aes_context(self._ctx)
    
    def set_enc_key(self, key):
        """
        Sets key used for encryption.
        Note that calling 'set_dec_key' will override this setting.
        """
        key_buf = ctypes.create_string_buffer(key, len(key))
        if lib.aes_setkey_enc(self._ctx, key_buf, len(key) * 8):
            raise RuntimeError("Invalid key")
    
    def set_dec_key(self, key):
        """
        Sets key used for decryption.
        Note that calling 'set_enc_key' will override this setting.
        """
        key_buf = ctypes.create_string_buffer(key, len(key))
        if lib.aes_setkey_dec(self._ctx, key_buf, len(key) * 8):
            raise RuntimeError("Invalid key")
    
    def _crypt(self, op, data):
        if len(data) % 16 != 0:
            raise RuntimeError("Input length must be multiple of 16")
        
        data_buf = ctypes.create_string_buffer(data, len(data))
        result_buf = ctypes.create_string_buffer('\x00', len(data))
        
        data_ptr = ctypes.addressof(data_buf)
        result_ptr = ctypes.addressof(result_buf)
        for i in range(0, len(data), 16):
            lib.aes_crypt_ecb(self._ctx, op, data_ptr, result_ptr)
            data_ptr += 16
            result_ptr += 16
        
        return result_buf.raw
        
    def encrypt(self, data):
        """
        Encrypts the data. Length of 'data' must be multiple of 16.
        """
        return self._crypt(AES.OP_ENCRYPT, data)
    
    def decrypt(self, data):
        """
        Decrypts the data. Length of 'data' must be multiple of 16.
        """
        return self._crypt(AES.OP_DECRYPT, data)
    