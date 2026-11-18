#!/bin/bash
# CA2 test runner. Run from the project root:  bash tests/run_tests.sh
# It rebuilds the program, deletes data/shelters.dat, runs every test in order
# and writes the output to tests/CA2_test_evidence.txt
g++ -std=c++11 -Wall -Wextra -o shelter src/main.cpp || exit 1
OUT=tests/CA2_test_evidence.txt
rm -f data/shelters.dat
echo "CA2 TEST EVIDENCE - Earthquake Shelter Occupancy Manager" > $OUT
echo "Generated: $(date)" >> $OUT

run() {   # run "<title>" "<input text>"
    echo "" >> $OUT
    echo "================================================================" >> $OUT
    echo "$1" >> $OUT
    echo "================================================================" >> $OUT
    echo "Keyboard input (each line = Enter): $(printf "$2" | tr "\n" "~" | sed "s/~/ , /g")" >> $OUT
    echo "----------------------------------------------------------------" >> $OUT
    printf "$2" | ./shelter | grep -v "^$" | grep -v "=====" | grep -v "^[0-5]\. \|^0\. " >> $OUT
}

run "T1  Empty file: view all, search, record number, update" \
    '2\n3\n1\n4\n5\n1\n0\n'

run "T2  Create 5 valid shelters (saved to file) + duplicate ID in the same run" \
    '1\n101\nGovt School\nSector 5\n200\n150\n1\n102\nCommunity Hall\nModel Town\n100\n100\n1\n103\nPark Tent\nNorth Zone\n50\n10\n1\n104\nSports Stadium\nGround Road\n1000\n850\n1\n105\nTemple Dharamshala\nOld City\n80\n30\n1\n101\n0\n'

run "T3  RESTART the program: data must still be there" \
    '2\n0\n'

run "T4  Invalid input and duplicate key" \
    '1\n101\n1\n0\n1\n-5\n1\nabc\n106\n\nHall\nArea\n0\n1\n106\nHall\nArea\n6000\n1\n106\nHall\nArea\n100\n150\n1\n106\nA very very long shelter name here\nRelief Camp\nEast Zone\n100\n10\n2\n0\n'

run "T5  Search by ID (seekg): found and not found" \
    '3\n103\n3\n999\n0\n'

run "T6  View by record number (random access with seekg)" \
    '4\n2\n4\n0\n4\n9\n4\n5\n0\n'

run "T7  Update occupancy (seekp): valid, invalid, unknown ID, then view all in the same run" \
    '5\n101\n180\n5\n101\n500\n5\n101\n-1\n5\n999\n5\n103\n50\n2\n0\n'

run "T8  RESTART again: updates must be saved in the file" \
    '2\n0\n'

# T9: the data file disappears while the program is running; the copy-constructor
# backup puts the old value back in memory, so View all still shows the old number.
cp data/shelters.dat /tmp/shelters_backup.dat
echo "" >> $OUT
echo "================================================================" >> $OUT
echo "T9  Update when the data file is missing (copy constructor backup restores the value)" >> $OUT
echo "================================================================" >> $OUT
echo "Keyboard input: 5 , 103 , (data file deleted here) , 20 , 2 , 0" >> $OUT
echo "----------------------------------------------------------------" >> $OUT
(printf '5\n103\n'; sleep 1; rm -f data/shelters.dat; printf '20\n2\n0\n') | ./shelter | grep -v "^$" | grep -v "=====" | grep -v "^[0-5]\. " >> $OUT
cp /tmp/shelters_backup.dat data/shelters.dat

run "T10 Menu: invalid choice (9 and the removed option 6) and non-number" \
    '9\nxyz\n6\n0\n'

echo "" >> $OUT
echo "================================================================" >> $OUT
echo "FILE CHECK: size of data/shelters.dat (6 records x 72 bytes = 432)" >> $OUT
echo "================================================================" >> $OUT
ls -l data/shelters.dat | awk '{print $5 " bytes"}' >> $OUT
echo "" >> $OUT
echo "First 72 bytes (record 1) in hex:" >> $OUT
xxd -l 72 data/shelters.dat >> $OUT
echo "Done. Evidence written to $OUT"
