import numpy as np

def clamp_pressure(src, low, high):
    """
    Clamp the pressure values in `src` to lie within a level-dependent range and store in `tgt`.

    Parameters:
    - src: 2D numpy array (source array)
    - low: float (lower bound for clamping)
    - high: float (upper bound for clamping)

    Returns:
    - tgt: 2D numpy array (clamped array)
    """
    # Dimensions of the source array
    m, n = src.shape

    # Initialize target array and level-dependent clamping limits
    tgt = np.zeros_like(src)
    tgt_min = low + np.arange(n) * np.finfo(src.dtype).eps
    tgt_max = high - np.arange(n - 1, -1, -1) * np.finfo(src.dtype).eps

    # Apply clamping for each element
    for j in range(n):
        tgt[:, j] = np.minimum(tgt_max[j], np.maximum(tgt_min[j], src[:, j]))

    return tgt

def clamp_temperature(src, low, high):
    """
    Clamp the temperature values in `src` to lie within [low, high] and store in `tgt`.

    Parameters:
    - src: 2D numpy array (source array)
    - low: float (lower bound for clamping)
    - high: float (upper bound for clamping)

    Returns:
    - tgt: 2D numpy array (clamped array)
    """
    # Apply clamping to the entire array
    tgt = np.minimum(high, np.maximum(low, src))
    return tgt

def main():
    # Test the clamp_pressure function
    src = np.array([[0.0, 1.0, 2.0], [3.0, 4.0, 5.0]])
    tgt = clamp_pressure(src, 0.5, 4.5)
    # check the output
    tgt_check = np.array([[0.5, 1.0,  2.0, ], [3.0,  4.0,  4.5]])
    if np.allclose(tgt, tgt_check):
        print("Test passed.")

    # Test the clamp_temperature function
    src = np.array([[0.0, 1.0, 2.0], [3.0, 4.0, 5.0]])
    tgt = clamp_temperature(src, 1.0, 4.0)
    tgt_check = np.array([[1.0, 1.0, 2.0], [3.0, 4.0, 4.0]])
    if np.allclose(tgt, tgt_check):
        print("Test passed.")

if __name__ == '__main__':
    main()
