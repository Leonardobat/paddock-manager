#!/bin/bash

SQLITE_VERSION="3470200"
SQLITE_YEAR="2024"
SQLITE_FILE="sqlite-amalgamation-${SQLITE_VERSION}.zip"
SQLITE_URL="https://www.sqlite.org/${SQLITE_YEAR}/${SQLITE_FILE}"
SQLITE_DIR="libs/sqlite3"

# Function to check if SQLite files exist and are valid
check_sqlite_files() {
    if [ -f "${SQLITE_DIR}/sqlite3.c" ] && [ -f "${SQLITE_DIR}/sqlite3.h" ]; then
        if sha256sum -c "${SQLITE_DIR}/sqlite3_hash.txt" &>/dev/null; then
            echo "SQLite files are present and valid."
            return 0
        else
            echo "SQLite files are present but invalid."
        fi
    fi
    return 1
}

# Check if SQLite files already exist and are valid
if check_sqlite_files; then
    exit 0
fi

# Download SQLite amalgamation
echo "Downloading SQLite amalgamation..."
curl -L -o "${SQLITE_FILE}" "${SQLITE_URL}"

# Unzip the file
echo "Extracting SQLite files..."
unzip -j "${SQLITE_FILE}" "sqlite-amalgamation-${SQLITE_VERSION}/sqlite3.c" "sqlite-amalgamation-${SQLITE_VERSION}/sqlite3.h" "sqlite-amalgamation-${SQLITE_VERSION}/sqlite3ext.h" -d "${SQLITE_DIR}"

# Remove the zip file
rm "${SQLITE_FILE}"

# Verify the files
if check_sqlite_files; then
    echo "SQLite files downloaded and verified successfully."
    exit 0
else
    echo "Failed to verify downloaded SQLite files."
    exit 1
fi