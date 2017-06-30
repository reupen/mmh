#pragma once

namespace mmh {

#if 0
	// Meh. This things always formats with decimal places (unless you pass a custom NUMBERFMT which means bypassing the user's own settings anyway).
	class format_integer : public pfc::stringcvt::string_utf8_from_wide {
	public:
		format_integer(t_uint64 number)
		{
			wchar_t buffer[65] = { 0 };

			if (!_ui64tow_s(number, buffer, tabsize(buffer), 10))
			{
				int size = GetNumberFormat(LOCALE_USER_DEFAULT, 0, buffer, NULL, NULL, 0);

				if (size > 0)
				{
					pfc::array_t<wchar_t> formatted;
					formatted.set_size((t_size)size);
					formatted.fill_null();

					if (GetNumberFormat(LOCALE_USER_DEFAULT, 0, buffer, NULL, formatted.get_ptr(), size))
					{
						convert(formatted.get_ptr(), formatted.get_size());
					}
				}
			}

		}
	};

#else
	class format_integer : public pfc::string_formatter {
	public:
		format_integer(t_uint64 size)
		{
			// Calculate number of digits safely.
			t_size digits = 0;
			t_uint64 size_temp = size;

			while (size_temp)
			{
				size_temp /= 10;
				digits++;
			}

			// Get thousands separator
			WCHAR wide_separator[4] = L",";
			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, wide_separator, tabsize(wide_separator));
			pfc::stringcvt::string_utf8_from_wide separator(wide_separator, tabsize(wide_separator));
			t_size separator_len = strlen(separator.get_ptr());

			// Format the number
			if (!digits) ++digits;

			t_size separators = (digits - 1) / 3;

			pfc::string_buffer buffer(*this, digits + separators*separator_len);

			t_size pos = digits + separators*separator_len - 1;
			size_temp = size;

			for (t_size i = 0; i < digits; i++)
			{
				buffer[pos] = pfc::format_hex_char(size_temp % 10);
				size_temp /= 10;
				// In theory we should use LOCALE_SGROUPING...
				if (i && i + 1 < digits && (i + 1) % 3 == 0)
				{
					PFC_ASSERT(pos > separator_len);
					pos -= separator_len;
					memcpy(&buffer[pos], separator, separator_len);
				}
				pos--;
			}

		}
	};

#endif
	const char * g_convert_utf16_to_ascii(const WCHAR* str_utf16, t_size len, pfc::string_base& p_out);
	const char * g_convert_utf8_to_ascii(const char* p_source, pfc::string_base& p_out);
    /**
     * \brief Partially compares two UTF-8 strings.
     * 
     * Replacement for stricmp_utf8_partial in shared.dll (based on similar functions in pfc).
     * 
     * If the substring is shorter than the main string, and the two strings are equal until that point,
     * they are considered equal for the purposes of this function. Otherwise, this functions like a 
     * normal case-insensitive string comparison.
     * 
     * \param str       The main string being checked against a substring
     * \param substr    The substring to compare the main string with
     * \return          0 if considered equal
     *                  -1 if str < substr
     *                  1 if str > substr
     */
    int compare_string_partial_case_insensitive(const char* str, const char* substr);

	char format_digit(unsigned p_val);

	class format_uint_natural {
	public:
		format_uint_natural(t_uint64 p_val, unsigned p_width = 0, unsigned p_base = 10)
			: m_value(p_val)
		{
			enum { max_width = tabsize(m_buffer) - 1 };

			if (p_val < 10)
			{
				if (p_val == 0)
					strcpy_s(m_buffer, "zero");
				else if (p_val == 1)
					strcpy_s(m_buffer, "one");
				else if (p_val == 2)
					strcpy_s(m_buffer, "two");
				else if (p_val == 3)
					strcpy_s(m_buffer, "three");
				else if (p_val == 4)
					strcpy_s(m_buffer, "four");
				else if (p_val == 5)
					strcpy_s(m_buffer, "five");
				else if (p_val == 6)
					strcpy_s(m_buffer, "six");
				else if (p_val == 7)
					strcpy_s(m_buffer, "seven");
				else if (p_val == 8)
					strcpy_s(m_buffer, "eight");
				else if (p_val == 9)
					strcpy_s(m_buffer, "nine");
				//else if (p_val == 10)
				//strcpy_s(m_buffer, "ten");
			}
			else
			{

				if (p_width > max_width) p_width = max_width;
				else if (p_width == 0) p_width = 1;

				char temp[max_width];

				unsigned n;
				for (n = 0; n < max_width; n++)
				{
					temp[max_width - 1 - n] = format_digit((unsigned)(p_val % p_base));
					p_val /= p_base;
				}

				for (n = 0; n < max_width && temp[n] == '0'; n++) {}

				if (n > max_width - p_width) n = max_width - p_width;

				char * out = m_buffer;

				for (; n < max_width; n++)
					*(out++) = temp[n];
				*out = 0;
			}
		}
		inline const char * get_ptr() const { return m_buffer; }
		inline const char * toString() const { return m_buffer; }
		inline operator const char*() const { return m_buffer; }
		bool is_plural() { return m_value != 1; }
	private:
		char m_buffer[64];
		t_uint64 m_value;
	};

	t_size poweroften(t_size raiseTo);

	class format_file_size : public pfc::string_formatter {
	public:
		format_file_size(t_uint64 size)
		{
			t_uint64 scale = 1024;
			const char * unit = "kB";
			const char * const unitTable[] = { "B","kB","MB","GB","TB" };
			for (t_size walk = 2; walk < tabsize(unitTable); ++walk)
			{
				t_uint64 next = scale * 1024;
				if (size < next) break;
				scale = next; unit = unitTable[walk];
			}
			uint64_t major = (size / scale), minor = 0;
			t_size minor_digits = 0;

			bool b_minor = major <= 99 && size;

			uint64_t remainder_raw = size % scale; t_size i = 0, j;

			uint64_t remainder = (remainder_raw * 1000) / scale;

			//while (remainder > poweroften(i)) i++;
			//if (i) i--;
			i = 3;

			if (b_minor)
			{
				minor_digits = 1;
				if (major < 10)
					minor_digits++;

				minor = remainder / (poweroften(i - minor_digits));

				j = minor_digits;

				/*for (j=0; j<minor_digits; j++)
				{
				minor *= 10;
				if (i>j)
				minor += (remainder ) / poweroften(i-j-1);
				}*/
				if (i > minor_digits && ((remainder % poweroften(i - minor_digits)) / poweroften(i - minor_digits - 1)) >= 5)
				{
					if ((minor % 10 < 9) || (minor_digits == 2 && (((minor % 100) / 10) < 9)))
						minor++;
					else { major++; minor = 0; }
				}
			}
			else if (i && ((remainder /*% poweroften(i-1)*/) / poweroften(i - 1)) >= 5)
				major++;

			*this << major;
			if (b_minor)
			{
				WCHAR separator[4] = { '.',0,0,0 };
				GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, separator, tabsize(separator));
				*this << pfc::stringcvt::string_utf8_from_wide(separator, tabsize(separator)) << pfc::format_uint(minor, minor_digits);
			}

			*this << " " << unit;
			m_scale = scale;
		}
		t_uint64 get_used_scale() const { return m_scale; }
	private:
		t_uint64 m_scale;
	};

}