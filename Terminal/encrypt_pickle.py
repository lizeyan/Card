from simplecrypt import encrypt, decrypt
import pickle
import settings
import os


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

