#pragma once

template <typename T>
inline T min(T a, T b) {
	return (a < b) ? a : b;
}

template <typename T>
inline T max(T a, T b) {
	return (a > b) ? a : b;
}

template <typename T>
inline T clamp(T x, T mn, T mx) {
	return max(min(x, mx), mn);
}
