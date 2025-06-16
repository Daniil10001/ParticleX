import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from collections import defaultdict
from scipy.integrate import quad

def parse_file(filename):
    data = []
    d=[]
    with open(filename, 'r') as file:
        for line in file:
            time_part, points_part = line.strip().split('|', 1)
            time = float(time_part)
            points_data = []
            records = points_part.split(';')
            
            for record in records:
                record = record.strip()
                if not record or '>' not in record:
                    continue
                
                coords_str, vel_str = record.rsplit('>', 1)
                coords = coords_str.replace('<', '').strip()
                try:
                    # Нам нужна только энергия для гистограммы
                    vel = float(vel_str.strip())**0.5
                    points_data.append(vel)
                except ValueError:
                    continue
            #d.append(sum(np.array(points_data)**2))
            data.append((time, points_data))
    #plt.plot(d)
    #plt.show()
    return data

def Maksvell2D(Mass, Temp, velocity, deltV, N, p):
    return np.vectorize(lambda vel: quad(lambda v: np.sum(p*(Mass/(1.38e-23*Temp))*np.exp(-Mass*v*v/(2*1.38e-23*Temp)))*v,vel,vel+deltV)[0]*N)(velocity)

# Считываем данные из файла
parsed_data = parse_file('res.txt')

from scipy.special import lambertw
def get_real_mass(ms,ws):
    ms=np.array(ms)
    ws=np.array(ws)
    return -np.real(lambertw(np.sum(ws*ms*np.exp(-ms))))

# Собираем все значения энергии для определения границ гистограммы
all_energies = []
#delim=[(0,2000),(2000,4000)]
delims=[[(0,500),(1000,1500),(2000,2500),(3000,3500)],[(500,1000),(1500,2000),(2500,3000),(3500,4000)]]
masses=[1/6e23,4/6e23]
delims=[[(0,4000)]]
masses=[np.array([2,18])/6e23]
pbs=[np.array([0.5,0.5])]
#print(get_real_mass([1,4],[0.5,0.5]))
ll=[sum([e-s for s,e in delim])  for delim in delims]

for time, energies in parsed_data:
    all_energies.extend([list(np.concatenate([energies[s:e] for s,e in delim])) for delim in delims])

Vmn=[sum(all_energies[i])/sum(ll) for i in range(len(all_energies))]
plt.plot(Vmn)
mk=(1/(0.5/2**0.5+0.5/18**0.5))**2
print(mk)
plt.plot([0,len(all_energies)],[np.sqrt(np.pi*8.31*300/(2*mk))]*2)
plt.show()

# Создаем бины для гистограммы
min_energy = min(min(all_energies))
max_energy = max(max(all_energies))
print(min_energy, max_energy)
num_bins = 50
bin_width = (max_energy - min_energy) / num_bins
bins = np.linspace(0, max_energy + bin_width, num_bins + 1)

# Предварительно вычисляем гистограммы для каждого момента времени
histograms = []
for time, energies in parsed_data:
    hist = [np.histogram(np.concatenate([energies[s:e] for s,e in delim]), bins=bins)[0]  for delim in delims]
    histograms.append(hist)

# Создаем фигуру и оси
fig, ax = plt.subplots(figsize=(10, 6))
ax.set_title('Распределение энергии частиц по скоростям')
ax.set_xlabel('Скорость')
ax.set_ylabel('Количество частиц')
ax.grid(True, linestyle='--', alpha=0.7)

# Находим максимальное значение по оси Y для фиксированного масштаба
max_count = max([max(hist) for hist in histograms[-1]])*1.2
ax.set_ylim(0, max_count)

# Центры бинов для отрисовки
bin_centers = (bins[:-1] + bins[1:]) / 2
width = bin_width * 0.8

# Создаем объект столбцов гистограммы
color=['blue','red']
bbars = [ax.bar(bin_centers, hist, width=width, 
              align='center', alpha=0.7, color=color[i]) for i,hist in enumerate(histograms[0])]

plots=[ax.plot(bins, Maksvell2D(masses[i],300,bins,bin_width,d,pbs[i]), color=color[i]) for i,d in enumerate(ll)]
print(bins)
#print(Maksvell2D(masses[0],273,bins,bin_width,2000))
# Текстовые элементы
time_text = ax.text(0.02, 0.95, '', transform=ax.transAxes, fontsize=12)
count_text = ax.text(0.02, 0.90, '', transform=ax.transAxes, fontsize=10)

def init():
    """Инициализация анимации"""
    for bars in bbars:
        for bar in bars:
            bar.set_height(0)
    time_text.set_text('')
    count_text.set_text('')
    return bars, time_text, count_text

def update(frame):
    """Обновление кадра анимации"""
    time, energies = parsed_data[frame]
    hist = histograms[frame]
    
    # Обновляем высоты столбцов
    for bi,bars in enumerate(bbars):
        for i, bar in enumerate(bars):
            bar.set_height(hist[bi][i])
    
    # Обновляем текст
    time_text.set_text(f'Время: {time:.6f} с')
    count_text.set_text(f'Частиц: {len(energies)}')
    
    return bars, time_text, count_text

# Создаем анимацию
ani = FuncAnimation(
    fig, 
    update, 
    frames=len(parsed_data),
    init_func=init,
    blit=False,
    interval=20,  # 200 мс между кадрами
    repeat_delay=2000  # 2 секунды перед повторением
)

plt.tight_layout()
plt.show()

# Для сохранения анимации в файл (раскомментируйте при необходимости)
#ani.save('vel_histogram.gif', writer='pillow', fps=5)