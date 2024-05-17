#pragma once
static class TestLibrary
{
	template <typename TFunc> void RunAndMeasure(const char* title, TFunc func)
	{
		const auto start = std::chrono::steady_clock::now();
		auto ret = func();
		const auto end = std::chrono::steady_clock::now();
		cout << title << ": " << std::chrono::duration <double, std::milli>(end - start).count() << " ms, res " << ret << "\n";
	}
};

