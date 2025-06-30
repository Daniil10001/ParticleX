import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import matplotlib.colors as mcolors
from matplotlib import cm
from matplotlib.cm import ScalarMappable

def parse_file(filename):
    data = []
    with open(filename, 'r') as file:
        for line in file:
            time_part, points_part = line.strip().split('|', 1)
            time = float(time_part)
            particles = []
            records = points_part.split(';')
            
            for i, record in enumerate(records):
                record = record.strip()
                if not record or '>' not in record:
                    continue
                
                coords_str, energy_str = record.rsplit('>', 1)
                coords = coords_str.replace('<', '').strip()
                try:
                    x1, x2 = map(float, coords.split(','))
                    v2 = float(energy_str.strip())
                    particles.append((x1, x2, i, v2))  # Сохраняем индекс частицы
                except ValueError:
                    continue
            
            data.append((time, particles))
    
    return data

# Параметры сетки
GRID_X_MIN = 1
GRID_X_MAX = 2
GRID_Y_MIN = 1
GRID_Y_MAX = 1.4
CELL_SIZE = 0.1  # Размер ячейки сетки

# Определение сетки
x_bins = np.arange(GRID_X_MIN, GRID_X_MAX + CELL_SIZE, CELL_SIZE)
y_bins = np.arange(GRID_Y_MIN, GRID_Y_MAX + CELL_SIZE, CELL_SIZE)
grid_shape = (len(y_bins) - 1, len(x_bins) - 1)

# Настройки типов частиц (индексы)
PARTICLE_TYPES = {
    "All": [(0, 2000)]      # Все частицы
}

# Цвета для разных типов частиц
TYPE_Mass = {
    "All": 29e-3/6e23,
}

# Считываем данные
parsed_data = parse_file('./gr/term4.txt')

# Создаем фигуру с несколькими субплогами
fig, axes = plt.subplots(1, len(PARTICLE_TYPES), figsize=(5*len(PARTICLE_TYPES), 5))
if len(PARTICLE_TYPES) == 1:
    axes = [axes]  # Для случая с одним типом

fig.suptitle('Температурная карта концентрации частиц', fontsize=16)

# Создаем изображения для каждого типа
images = []
for ax, (type_name, _) in zip(axes, PARTICLE_TYPES.items()):
    # Начальное пустое изображение
    img = ax.imshow(np.zeros(grid_shape), 
                   extent=[GRID_X_MIN, GRID_X_MAX, GRID_Y_MIN, GRID_Y_MAX],
                   origin='lower', 
                   aspect='auto',
                   cmap='viridis',
                   vmin=0, vmax=800)  # Фиксированный масштаб
    
    ax.set_title(f'{type_name} particles')
    ax.set_xlabel('X координата')
    ax.set_ylabel('Y координата')
    ax.minorticks_on()
    ax.grid(True, which='major', linestyle='-', alpha=0.7)  # Основная сетка
    ax.grid(True, which='minor', linestyle=':', alpha=0.4) 
    images.append(img)
    
    # Добавляем colorbar для каждого субплота
    cbar = fig.colorbar(img, ax=ax)
    cbar.set_label('Температура, $K$')

# Текстовый элемент для времени
time_text = fig.text(0, 0.95+0.11, '', ha='left', transform=axes[0].transAxes, fontsize=12)

def init():
    """Инициализация анимации"""
    for img in images:
        img.set_data(np.zeros(grid_shape))
    time_text.set_text('')
    return images + [time_text]

def update(frame):
    """Обновление кадра анимации"""
    time, particles = parsed_data[frame]
    print(frame)
    # Создаем тепловые карты для каждого типа частиц
    for i, (type_name, seidxs) in enumerate(PARTICLE_TYPES.items()):
        # Фильтруем частицы по типу
        filtered_particles = np.concatenate([
            [(x1, x2,v2) for x1, x2, idx, v2 in particles 
            if start_idx <= idx < end_idx] for start_idx,end_idx in seidxs])
        
        if filtered_particles.shape[0]==0:
            heatmap = np.zeros(grid_shape)
        else:
            # Преобразуем в массивы
            x_coords = np.array([p[0] for p in filtered_particles])
            y_coords = np.array([p[1] for p in filtered_particles])
            T_w=np.array([p[2] for p in filtered_particles])*TYPE_Mass[type_name]/(2*1.38e-23)
            
            # Строим 2D гистограмму
            heatmap, _, _ = np.histogram2d(
                y_coords, x_coords,  # Обратите внимание на порядок: y, x
                bins=[y_bins, x_bins], weights=T_w)
            count, _, _ = np.histogram2d(y_coords, x_coords,  # Обратите внимание на порядок: y, x
                bins=[y_bins, x_bins])
        
        # Нормализуем по размеру ячейки
        heatmap = np.nan_to_num(heatmap / count, nan=0)
        
        # Обновляем изображение
        images[i].set_data(heatmap)
        
        # Автомасштабирование (раскомментируйте при необходимости)
        # current_max = np.max(heatmap)
        # if current_max > 0:
        #     images[i].set_clim(0, current_max * 1.1)
    
    # Обновляем время
    time_text.set_text(f'Время: {time:.6f} с | Частиц: {len(particles)}')
    
    return images + [time_text]

# Создаем анимацию
ani = FuncAnimation(
    fig, 
    update, 
    frames=len(parsed_data),
    init_func=init,
    blit=False,
    interval=300,
    repeat_delay=2000
)

plt.tight_layout()  # Учитываем место для верхнего текста rect=[0, 0, 1, 0.95]
plt.show()

# Для сохранения анимации (раскомментируйте при необходимости)
ani.save('./gr/term4T.mp4', writer='ffmpeg', fps=10, dpi=100)

'''from matplotlib.animation import FFMpegWriter, PillowWriter
writer = FFMpegWriter(fps=5,bitrate=1000)
with writer.saving(fig, "particle_density.mp4", dpi=100):
    # Инициализируем первый кадр
    artists = init()
    writer.grab_frame()
    
    # Последовательно обрабатываем все кадры
    for frame in range(len(parsed_data)):
        artists = update(frame)
        writer.grab_frame()
        
        # Периодически выводим прогресс
        if frame % 10 == 0:
            print(f"Обработано кадров: {frame+1}/{len(parsed_data)}")'''