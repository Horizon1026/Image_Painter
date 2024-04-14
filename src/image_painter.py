import numpy as np
from scipy.stats import chi2
import matplotlib.pyplot as plt
import numpy as np

def get_ab_of_ellipse(matrix, alpha):
    eigenvalues, eigenvectors = np.linalg.eig(matrix)
    diag = np.array([[eigenvalues[0], 0], [0, eigenvalues[1]]])

    degrees_of_freedom = 2
    chi2_upper_quantile = chi2.ppf(1 - alpha, degrees_of_freedom)

    a = np.sqrt(np.sqrt(eigenvalues[0])*chi2_upper_quantile)
    b = np.sqrt(np.sqrt(eigenvalues[1])*chi2_upper_quantile)

    return a,b,eigenvectors

def draw(a,b,matrix, u):
    center = (0, 0)
    width = a
    height = b
    angle = 0

    theta = np.linspace(0, 2*np.pi, 100)
    x = center[0] + width/2 * np.cos(np.radians(angle)) * np.cos(theta) - height/2 * np.sin(np.radians(angle)) * np.sin(theta)
    y = center[1] + width/2 * np.sin(np.radians(angle)) * np.cos(theta) + height/2 * np.cos(np.radians(angle)) * np.sin(theta)

    #rotate
    nx = []
    ny = []
    u1 = u[0]
    u2 = u[1]

    for k in range(len(x)):
        vec = np.array([x[k], y[k]])
        vec = matrix.dot(vec)
        nx.append(vec[0]+u1)
        ny.append(vec[1]+u2)

    x = np.array(nx)
    y = np.array(ny)

    plt.figure(figsize=(6, 6))
    plt.plot(x, y, label='Ellipse')

    plt.axis('equal')

    plt.xlabel('X-axis')
    plt.ylabel('Y-axis')
    plt.title('Ellipse in Matplotlib')

    plt.legend()

    plt.show()

matrix = np.array([[2, 1], [1, 3]])
u = [1,2]
alpha = 0.6065

a,b,matrix = get_ab_of_ellipse(matrix, alpha)
draw(a,b, matrix, u)