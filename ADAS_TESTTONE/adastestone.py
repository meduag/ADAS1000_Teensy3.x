import matplotlib.pyplot as plt
import numpy as np

file_name = "ejemplo2.txt"
raw_f = open(file_name, 'r')
arra_add = 0

y_vector11 = []
y_vector12 = []
y_vector13 = []
y_vector14 = []
y_vector15 = []

c = 8

for x in raw_f:
    xx = ""
    if x != '\n':
        xx = x.rstrip()
        ##print(xx)

        y_vector11.append(int(xx[2:c],16))
        y_vector12.append(int(xx[c + 1 +2:(c*2)+1],16))
        y_vector13.append(int(xx[(c*2) + 2+2:(c*3)+2],16))
        y_vector14.append(int(xx[(c * 3)+3+2:(c*4)+3],16))
        y_vector15.append(int(xx[(c * 4)+4+2:(c*5)+4],16))

tam = len(y_vector11)
x_vector = np.linspace(0, tam - 1, tam)

plt.plot(x_vector, y_vector12)
plt.show()
"""plt.plot(x_vector, y_vector12)
plt.show()
plt.plot(x_vector, y_vector13)
plt.show()
plt.plot(x_vector, y_vector14)
plt.show()
plt.plot(x_vector, y_vector15)
plt.show()"""

