import pickle
from Crypto.Cipher import AES
from Crypto.Util import Counter
import settings
import os


def encrypt(key, data: bytes):
    counter = Counter.new(128, initial_value=settings.IV.encode("utf-8"))
    cipher = AES.new(key, AES.MODE_CTR, counter=counter)
    encrypted = cipher.encrypt(data)
    return encrypted


def decrypt(key, data: bytes):
    counter = Counter.new(128, initial_value=settings.IV.encode("utf-8"))
    cipher = AES.new(key, AES.MODE_CTR, counter=counter)
    return cipher.decrypt(data)


def dump_encrypt(obj, filename: str):
    data = pickle.dumps(obj)
    encrypted = encrypt(settings.KEY, data)
    with open(filename, "wb") as f:
        f.write(encrypted)


def load_decrypt(filename):
    if not os.path.exists(filename) or not os.path.isfile(filename):
        return None
    with open(filename, "rb") as f:
        encrypted = f.read()
    data = decrypt(settings.KEY, encrypted)
    return pickle.loads(data)

if __name__ == '__main__':
    eptd = encrypt(settings.KEY[:32], "00000".encode("utf-8"))
    dptd = decrypt(settings.KEY[:32], eptd)
    print(dptd)
