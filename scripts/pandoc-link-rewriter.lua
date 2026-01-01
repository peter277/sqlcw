--[[
Pandoc filter to rewrite relative links to versioned GitHub URLs.

Reads BUILD_VERSION from environment:
  - Set to version number: links to /blob/v{version}/
  - Set to "dev" or unset: links to /blob/main/

Usage example: pandoc input.md -o out.html --lua-filter=scripts/pandoc-link-rewriter.lua
--]]

-- Read version from environment, fallback to 'main'
local version = os.getenv("BUILD_VERSION") or os.getenv("BUILD_VERSION_STR")
local base_url

-- Use main branch if no version, or if version is 'dev'
if version and version ~= "dev" then
  -- Use tag URL: /blob/v1.1.1/
  io.stderr:write("Using version tag: v" .. version .. "\n")
  base_url = "https://github.com/peter277/sqlcw/blob/v" .. version .. "/"
else
  -- Fallback to main branch (no version or dev build)
  if version == "dev" then
    io.stderr:write("Development build detected, using main branch\n")
  else
    io.stderr:write("No version found, using main branch\n")
  end
  base_url = "https://github.com/peter277/sqlcw/blob/main/"
end

function Link(el)
  -- Only rewrite relative links (not absolute URLs or anchors)
  if not el.target:match("^https?://") and
     not el.target:match("^#") and
     not el.target:match("^mailto:")
   then
    el.target = base_url .. el.target
  end
  return el
end
