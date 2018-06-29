# Report

`report` directory contains `evo_ape` and `evo_traj` reports. GNUPlot's top and side view images are also added for quick visual inspection

## Covariance

When we add external pose to bundle adjustment, each pose should be associated with covariance values.
Below hard coded covariance values are used

covariance for binary edge:
```
1.0 / 100.0
```
covariance for unary edge:
```
[
    [1.0/100.0          0.0                 0.0]
    [0.0                1.0/100.0           0.0]
    [0.0                0.0           1.0/100.0]
]
```

## Folder description

### Root folders description
- `unary`: contains all experiments done using unary edges only
- `binary`: contains all experiments done using binary edges only
- `unary_and_binary`: contains all experiments done using both unary and binary edges

### Sub-folders description
- `all_frame_pos`: contains results where all frames are provided with external pose
- `init_100_skip_10`: contains results where initial `100` frames are provided with external pose and later every `10` frames are skipped from external pose

    E.g. provided `0-99` frames with external pose, do not provide external pose to `100-110`. Provide external pose to `111` frame, do not provide external pose to  skip `112-122` etc.
- `init_100_skip_20`: contains results where initial `100` frames are provided with external pose and later every `20` frames are skipped from external pose
- `init_100_skip_rest`: contains results where initial `100` frames are provided with external and the external poses are skipped for rest of the frames
