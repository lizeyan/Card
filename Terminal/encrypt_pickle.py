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
    encrypted = encrypt(settings.KEY[:32], data)
    with open(filename, "wb") as f:
        f.write(encrypted)


def load_decrypt(filename):
    if not os.path.exists(filename) or not os.path.isfile(filename):
        return None
    with open(filename, "rb") as f:
        encrypted = f.read()
    data = decrypt(settings.KEY[:32], encrypted)
    return pickle.loads(data)


def money2int(money):
    money_100 = int(money *100)
    money_str = str(money_100)
    money_str = '0' * (8 - len(money_str)) + money_str
    eptd = encrypt(settings.KEY[:32], money_str.encode("utf-8"))
    ans = [0, 0]
    for i in range(2):
        ans[i] = int(eptd[i * 4]) + 256 * int(eptd[i * 4 + 1]) + 256 * 256 * int(eptd[i * 4 + 2]) + \
                 (256 ** 3) * int(eptd[i * 4 + 3])
    return ans


def int2money(money_int):
    money_int = [int(x) for x in money_int]
    money8 = []
    for i in range(2):
        for k in range(4):
            money8.append(int(money_int[i]) % 256)
            money_int[i] /= 256
    money8 = [x.to_bytes(1, byteorder='big') for x in money8]
    ans = money8[0]
    for i in range(1, 8):
        ans = ans + money8[i]
    dptd = decrypt(settings.KEY[:32], ans)
    ans = float(int(dptd.decode('utf-8'))) / 100
    return ans

if __name__ == '__main__':
    ans = money2int(1231.12)
    print(ans)
    money = int2money(ans)
    print(money)
