cd $1 || exit

# Run the two_step_sort benchmark and merge_partition benchmark and copy the results for each bench to the corresponding result csv file

# create csv if not exist
touch ./two_step_sort.csv
touch ./merge_partition.csv

# clear the result csv file
echo "" >./two_step_sort.csv
echo "" >./merge_partition.csv

for i in {16..21}; do
  # run and redict the output to the corresponding csv file
  echo "RUNNING ITER $i"
  ./two_step_sort_bench "$i" >>./two_step_sort.csv
  ./merge_partition_bench "$i" >>./merge_partition.csv
done

echo "TWO STEP SORT"
cat ./two_step_sort.csv

echo "PARTIAL MERGE"
cat ./merge_partition.csv
