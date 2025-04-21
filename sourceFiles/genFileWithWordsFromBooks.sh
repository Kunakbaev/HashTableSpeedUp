#! /bin/bash

set -eu

BOOKS_SOURCE_DIR="books"
FILES_WITH_WORDS_DEST_DIR="words"
ALL_WORDS_FILE_NAME="allWordsFromBooks"
MIN_WORD_LEN=3

echo "parsing words from books located at $BOOKS_SOURCE_DIR" 

function parseWordsFromFile() {
    grep -Eow "\w{$MIN_WORD_LEN,}" "$1" > "$2"
}

function generateAllWordFiles() {
    for bookFileName in `ls $BOOKS_SOURCE_DIR`; do
        echo "bookFileName : $bookFileName";
        fileName="$BOOKS_SOURCE_DIR/$bookFileName"
        outputFileName="$FILES_WITH_WORDS_DEST_DIR/words_$bookFileName"
        parseWordsFromFile $fileName $outputFileName
    done
}

function concatWordFilesIntoOne() {
    IFS=$'\n'
    tmp=$(ls $FILES_WITH_WORDS_DEST_DIR)
    fileNames=()
    #echo "fileNames: ${fileNames[@]}"

    for f in ${tmp[@]}; do
        # echo "f : $f"
        fileNames+=("$FILES_WITH_WORDS_DEST_DIR/$f")
    done

    cat "${fileNames[@]}" > $ALL_WORDS_FILE_NAME".txt"
}

generateAllWordFiles

echo "concatinating all words from all books into one file"
concatWordFilesIntoOne

cat "$ALL_WORDS_FILE_NAME.txt" "randWordsFile" | awk '{print tolower($0)}' > "allWords.txt"

echo "done"