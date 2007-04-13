#include "paragui.h"
#include <stdlib.h>

#ifndef HAVE_STRDUP
char *strdup(const char *s) {
	if(s == NULL) {
		return NULL;
	}

	int l = strlen(s) + 1;
	char *ret = (char *)malloc(l);
	if (ret) {
		strcpy(ret,s);
	}
	return ret;
}
#endif

#ifndef HAVE_FNMATCH
#include <errno.h>

/* Match STRING against the filename pattern PATTERN, returning zero
   if it matches, FNM_NOMATCH if not.  */
int fnmatch (const char *pattern, const char *string, int flags) {
	register const char *p = pattern, *n = string;
	register char c;

	if ((flags & ~__FNM_FLAGS) != 0) {
		errno = EINVAL;
		return (-1);
	}

	while ((c = *p++) != '\0') {
		switch (c) {
			case '?':
				if (*n == '\0')
					return (FNM_NOMATCH);
				else if ((flags & FNM_PATHNAME) && *n == '/')
					return (FNM_NOMATCH);
				else if ((flags & FNM_PERIOD) && *n == '.' &&
				         (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
					return (FNM_NOMATCH);
				break;

			case '\\':
				if (!(flags & FNM_NOESCAPE))
					c = *p++;
				if (*n != c)
					return (FNM_NOMATCH);
				break;

			case '*':
				if ((flags & FNM_PERIOD) && *n == '.' &&
				        (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
					return (FNM_NOMATCH);

				for (c = *p++; c == '?' || c == '*'; c = *p++, ++n)
					if (((flags & FNM_PATHNAME) && *n == '/') ||
					        (c == '?' && *n == '\0'))
						return (FNM_NOMATCH);

				if (c == '\0')
					return (0);

				{
					char c1 = (!(flags & FNM_NOESCAPE) && c == '\\') ? *p : c;
					for (--p; *n != '\0'; ++n)
						if ((c == '[' || *n == c1) &&
						        fnmatch (p, n, flags & ~FNM_PERIOD) == 0)
							return (0);
					return (FNM_NOMATCH);
				}

			case '[': {
					/* Nonzero if the sense of the character class is
					   inverted.  */
					register int nott;

					if (*n == '\0')
						return (FNM_NOMATCH);

					if ((flags & FNM_PERIOD) && *n == '.' &&
					        (n == string || ((flags & FNM_PATHNAME) && n[-1] == '/')))
						return (FNM_NOMATCH);

					/* Make sure there is a closing `]'.  If there isn't,
					   the `[' is just a character to be matched.  */
					{
						register const char *np;

						for (np = p; np && *np && *np != ']'; np++)
							;

						if (np && !*np) {
							if (*n != '[')
								return (FNM_NOMATCH);
							goto next_char;
						}
					}

					nott = (*p == '!' || *p == '^');
					if (nott)
						++p;

					c = *p++;
					while (1) {
						register char cstart = c, cend = c;

						if (!(flags & FNM_NOESCAPE) && c == '\\')
							cstart = cend = *p++;

						if (c == '\0')
							/* [ (unterminated) loses.  */
							return (FNM_NOMATCH);

						c = *p++;

						if ((flags & FNM_PATHNAME) && c == '/')
							/* [/] can never match.  */
							return (FNM_NOMATCH);

						if (c == '-' && *p != ']') {
							cend = *p++;
							if (!(flags & FNM_NOESCAPE) && cend == '\\')
								cend = *p++;
							if (cend == '\0')
								return (FNM_NOMATCH);
							c = *p++;
						}

						if (*n >= cstart && *n <= cend)
							goto matched;

						if (c == ']')
							break;
					}
					if (!nott)
						return (FNM_NOMATCH);

next_char:
					break;

matched:
					/* Skip the rest of the [...] that already matched.  */
					while (c != ']') {
						if (c == '\0')
							/* [... (unterminated) loses.  */
							return (FNM_NOMATCH);

						c = *p++;
						if (!(flags & FNM_NOESCAPE) && c == '\\')
							/* 1003.2d11 is unclear if this is right.  %%% */
							++p;
					}
					if (nott)
						return (FNM_NOMATCH);
				}
				break;

			default:
				if (c != *n)
					return (FNM_NOMATCH);
		}

		++n;
	}

	if (*n == '\0')
		return (0);

	return (FNM_NOMATCH);
}
#endif

