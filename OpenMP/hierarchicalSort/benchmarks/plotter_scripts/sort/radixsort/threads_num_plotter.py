import matplotlib.pyplot as plt
import numpy as np

labels = ['1', '2', '4', '6', '12']
seq = [0] * 5
parallel = [0] * 5
poolMode = [0] * 5

# read CSV from  ../results/threads_num.csv
with open('../../../results/sort/radixsort/threads_num.csv', 'r') as f:
    data = f.read()

# split CSV into lines
data = data.split('\n')

for i in range(len(data)):
    data[i] = data[i].split(',')

    seq[i] = "{:.2f}".format(float(data[i][0]))
    parallel[i] = "{:.2f}".format(float(data[i][1]))
    poolMode[i] = "{:.2f}".format(float(data[i][2]))

# convert each element to float
seq = [float(i) for i in seq]
parallel = [float(i) for i in parallel]
poolMode = [float(i) for i in poolMode]

# zero first element of omp_task and threadpool except the last one

# plot
x = np.arange(len(labels))
width = 0.5 / 2
margin = 0.12 / 3

fig, ax = plt.subplots()
rects3 = ax.bar(x - width - margin, poolMode, width, label='quick-sort', color='#4287f5')
rects1 = ax.bar(x, seq, width, label='sequential and in-place', color='#96173f')
rects2 = ax.bar(x + width + margin, parallel, width, label='parallel and in-place', color='#72de68')

ax.set_title('Time vs Number of threads (n = 10,000,000)')
ax.set_ylabel('Time (ms)')
ax.set_xlabel('Number of threads')

ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.legend()


def auto_label(rects):
    for rect in rects:
        height = rect.get_height()
        if height != 0:
            ax.annotate('{}'.format(height), xy=(rect.get_x() + rect.get_width() / 3,
                                                 height), xytext=(0, 3),
                        textcoords="offset points", ha='center', va='bottom')


auto_label(rects3)
auto_label(rects1)
auto_label(rects2)

fig.tight_layout()

plt.show()

# save to ../results/threads_num.png
fig.savefig('../../../results/sort/radixsort/threads_num.png')
