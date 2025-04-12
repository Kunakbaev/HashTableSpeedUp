import random

random.seed(228)

lines = []
with open("allWords.txt", "r") as f:
    lines = f.readlines()

print("number of words : ", len(lines))
print("num of unique words: ", len(set(lines)))

with open("queries.txt", "w") as f:
    numOfQueries = int(4e6)
    queries = []
    for _ in range(numOfQueries):
        if random.randint(1, 2) & 1:
            wordInd = random.randint(0, len(lines) - 1)
            f.write(lines[wordInd])
        else:
            wordLen = random.randint(3, 16)
            word = [chr(ord('a') + random.randint(0, 25)) for _ in range(wordLen)]
            f.write("".join(word) + "\n")
