# sqlcw ideas backlog

- **nonewline fix when preceded by comment** - What behavior do we want here? Should we still strip all newlines, or preserve them after comments?

- **INI-style config file support** - Utilize `parse_config_file()` library function for parsing. Implement special handling for `[top]` and `[bottom]` sections to define header/footer content.

- **Preserve special comments with strip option** - Keep `/*+ */` hint comments when using `-comments strip`. Consider adding `keepspec` multi-token option syntax: `-comments strip keepspec`
