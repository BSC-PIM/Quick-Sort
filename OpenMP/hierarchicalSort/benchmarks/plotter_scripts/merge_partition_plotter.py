import matplotlib.pyplot as plt
import numpy as np

# data from https://allisonhorst.github.io/palmerpenguins/


with open('../results/merge_partition.csv', 'r') as f:
    data = f.read()

data = data.split('\n')
data_len = len(data)
BASE = 8

species = [BASE * 2 ** i for i in range(data_len)]
species = [str(i) for i in species]

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

ax.set_title("Time vs Number of Workers (n = 10,000,000 unsigned 64-bit)")
ax.legend(loc="upper left")

# set name for x and y axis
ax.set_xlabel("Number of Workers")
ax.set_ylabel("Time (ms)")


def auto_label(rect):
    height = rect.get_height()
    if height != 0:
        ax.annotate('{}'.format(height), xy=(rect.get_x() + rect.get_width() / 3,
                                             height), xytext=(0, 3),
                    textcoords="offset points", ha='center', va='bottom')


# set log 2 scale for x axis

# set label for each bar
for rect in ax.patches:
    auto_label(rect)


fig.savefig('../results/merge_partition.png')

plt.show()

# save fig
