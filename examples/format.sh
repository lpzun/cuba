# for file in $(find . -name '*.spec'); do
#    echo "$file" 
#    echo "$file.old"
#    mv $file "$file.old"
# done

# # rename .pds.tmp file to .pds
#for file in $(find . -name '*.tmp'); do
#   echo "${file}"
#   echo "${file%.tmp}"
#   mv $file "${file%.tmp}"
#done

# # build input initial stats
# # build input specifications
for file in $(find . -name '*.pds'); do
   echo "${file}"
   echo "${file%.pds}.init"
   echo "0|0,0" > ${file%.pds}.init
   echo "${file%.pds}.spec"
   echo "0|0,0" > ${file%.pds}.spec
done
