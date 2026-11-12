# ConfigurableFirmata (tier B)

Optional path for Stepper / Encoder / Scheduler via ConfigurableFirmata rather than custom hub.

**Deferred by design** (roadmap phase 6: only if AccelStepper/encoder is required). Sensor and motor hubs cover the primary timing/motor gaps.

Use Standard Firmata + DeviceIO for Digital/PWM/Servo/I2C; keep catalog stub `configurable_firmata` with `available: false` until a concrete stepper sample lands.
