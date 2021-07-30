from hashlib import sha256

files = [
    "abc.txt", 
    "abcabc.txt", 
    "bible.txt", 
    "characters.txt", 
    "large.txt", 
    "lorem.txt", 
    "nothing.txt", 
    "test.txt"
]

for file in files:
    with open(f"texts/{file}", "r") as f:
        M = f.read()
        print(f"\n{file}:")
        print(sha256(M.encode('utf-8')).hexdigest())
