import matplotlib.pyplot as plt
import numpy as np

# data from https://allisonhorst.github.io/palmerpenguins/


with open('../results/merge_partition.csv', 'r') as f:
    data = f.read()

data = data.split('\n')
data_len = len(data)
BASE = 8

species = [BASE * 2 ** i for i in range(data_len)]
species = [str(i) + " workers" for i in species]

merge_partition = [data[i].split(',')[0] for i in range(data_len)]
sort_partition = [data[i].split(',')[1] for i in range(data_len)]

# convert to ndarray of float
merge_partition = np.array(merge_partition, dtype=float)
sort_partition = np.array(sort_partition, dtype=float)

weight_counts = {
    "Sort Partition": merge_partition,
    "Merge Partition": sort_partition,
}

width = 0.5

fig, ax = plt.subplots()
bottom = np.zeros(4)

for boolean, weight_count in weight_counts.items():
    p = ax.bar(species, weight_count, width, label=boolean, bottom=bottom)
    bottom += weight_count

ax.set_title("Time vs Number of Workers")
ax.legend(loc="upper left")

# set name for x and y axis
ax.set_xlabel("Number of Workers")
ax.set_ylabel("Time (ms)")

# set log 2 scale for x axis

plt.show()
