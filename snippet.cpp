
#include "snippet.h"


Snippet::Snippet() { id=0; name="<new snippet>"; snip.clear(); }
Snippet::Snippet(const Snippet &R) { id=R.id; name=R.name; snip=R.snip; }
Snippet::~Snippet() {}


Snippets::Snippets() { clear(); }
Snippets::~Snippets() { clear(); }
