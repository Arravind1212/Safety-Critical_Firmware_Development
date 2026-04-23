import numpy as np
from sklearn.model_selection import train_test_split
from xgboost import XGBRegressor
import m2cgen as m2c

# 1. Generate synthetic data: [voltage (V), current (A), temperature (°C)]
np.random.seed(42)
n_samples = 500

voltage = np.random.uniform(3.0, 4.2, n_samples)
current = np.random.uniform(-50, 50, n_samples)  # Charging = negative current
temperature = np.random.uniform(15, 45, n_samples)

# Define simple SOC logic: higher voltage → higher SOC, + corrections
soc = (
    (voltage - 3.0) / (4.2 - 3.0) * 100
    - 0.1 * current
    - 0.2 * (temperature - 25)
    + np.random.normal(0, 2, n_samples)
)
soc = np.clip(soc, 0, 100)

# 2. Prepare dataset
X = np.column_stack((voltage, current, temperature))
y = soc

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2)

# 3. Train XGBoost regressor
model = XGBRegressor(n_estimators=10, max_depth=3, learning_rate=0.1)
model.fit(X_train, y_train)

# 4. Export to C code using m2cgen
c_code = m2c.export_to_c(model)

# 5. Save to file
with open("soc_model.c", "w") as f:
    f.write(c_code)

print("Model trained and C code saved to 'soc_model.c'")
