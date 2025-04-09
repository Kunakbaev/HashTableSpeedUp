
MAX_LEN = 8

with open("sourceFiles/allWords.txt", "r") as f:
    cntLongWords = 0
    lines = f.readlines()

    for line in lines:
        if len(line) > MAX_LEN:
            print(line)
            cntLongWords += 1

    print(cntLongWords)