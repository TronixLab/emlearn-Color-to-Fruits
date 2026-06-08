// StandardScaler for Arduino - matches scikit-learn StandardScaler transform
#ifndef STANDARD_SCALER_H
#define STANDARD_SCALER_H

class StandardScaler {
public:
  static constexpr int n_features = 6;

  const float scaler_means[6] = { 0.423450f, 0.306633f, 0.269667f, 0.155833f, 0.436767f, 0.097383f };

  const float scaler_stds[6] = { 0.116613f, 0.056403f, 0.072089f, 0.196447f, 0.178292f, 0.044249f };
  
  // Scale input array in-place
  void transform(float *X, float *X_scaled) const {
    for (int i = 0; i < n_features; i++) {
      X_scaled[i] = (X[i] - scaler_means[i]) / scaler_stds[i];
    }
  }

  // Inverse scale input array in-place
  void inverse_transform(float *X_scaled, float *X) const {
    for (int i = 0; i < n_features; i++) {
      X[i] = X_scaled[i] * scaler_stds[i] + scaler_means[i];
    }
  }
};

#endif  // STANDARD_SCALER_H