#ifndef MIDIMON_UTILS_H
#define MIDIMON_UTILS_H

template <typename T>
class ConversionHelper
{
public:
	inline ConversionHelper(T *t) : m_object(t) {}

	template <typename U>
	inline operator U() const { return m_object; }

private:
	T *m_object;
};

template <typename T> ConversionHelper<T> static_alloc()
{
	static T t;
	return &t;
}

#endif // MIDIMON_UTILS_H
