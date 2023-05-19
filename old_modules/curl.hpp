/*
* Copyright (C) 2011, William H. Welna All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY William H. Welna ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL William H. Welna BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __CURLHEADER__
#define __CURLHEADER__
#include "main.hpp"
#include "curl-7.22.0\include\curl\curl.h"

#ifndef WINDOWS
static const char *cURL_lib_names[] = {
    "libcurl.so",
    "libcurl.so.3",
    "libcurl.so.4",
    "libcurl-gnutls.so",
    "libcurl-openssl.so",
    "libcurl-gnutls.so.4",
    "libcurl-gnutls.so.3",
    "libcurl-openssl.so.4",
    "libcurl-openssl.so.3",
    NULL
};
#endif
#ifdef WINDOWS
static const char *cURL_lib_names[] = {
    "libcurl.dll",
    NULL
};
#define dlopen(a,b) LoadLibrary(a)
#define dlsym(a,b) GetProcAddress(a,b)
#define dlclose(a) FreeLibrary(a)
#endif

class           Curl_NoLoadLib:public
    std::exception {
    virtual const char *
    what() const
    throw() {
	return "Curl: Unable to load libCURL";
    } int
    code() {
	return 1;
    }
};

class
    Curl_EasyInit:
    public
    std::exception {
    virtual const char *
    what() const
    throw() {
	return "Curl: curl_easy_init() failed";
    } int
    code() {
	return 2;
    }
};

class
    Curl {
  private:
    // Lib crap
#ifndef WINDOWS
    void           *
	cURL_lib;
#endif
#ifdef WINDOWS
    HINSTANCE__    *
	cURL_lib;
#endif
    CURL           *(*c_easy_init) (void);
    CURLcode(*c_easy_perform) (CURL *);
    CURLcode(*c_easy_setopt) (CURL *, CURLoption, ...);
    CURLcode(*c_easy_getinfo) (CURL *, CURLINFO, ...);
    CURLFORMcode(*c_formadd) (struct curl_httppost **,
			      struct curl_httppost **, ...);
    void            (*c_formfree) (struct curl_httppost *);
    struct curl_slist *(*c_slist_append) (struct curl_slist *,
					  const char *);
    void            (*c_slist_free_all) (struct curl_slist *);
    void            (*c_easy_cleanup) (CURL *);
    void            (*c_free) (void *);
    char           *(*c_easy_escape) (CURL *, char *, int);
    char           *(*c_easy_unescape) (CURL *, char *, int, int *);
    CURL           *
	ch;
    struct curl_httppost *
	f_first,
    *
	f_last;
    bool
	use_form,
	use_file,
	custom_header,
	is_debug;
    boost::regex
	r_co;
    boost::regex
	r_he;
    static size_t
    write_mem(void *p, size_t size, size_t nmemb, Curl * self) {
	self->page.append((char *) p, size * nmemb);
	return
	    nmemb;
    } static size_t
    write_file(void *p, size_t size, size_t nmemb, Curl * self) {
	self->file_out.write((char *) p, size * nmemb);
	return nmemb;
    }
    static size_t
    write_header(void *p, size_t size, size_t nmemb, Curl * self) {
	self->headerP.append((char *) p, size * nmemb);
	return nmemb;
    }
    void
    parse_header() {
	header.clear();
	string
	    t = headerP;
	boost::smatch match;
	if (boost::
	    regex_search(t, match, r_he, boost::match_not_dot_newline)) {
	    header["Version"] = match[1];
	    header["Code Number"] = match[2];
	    header["Code Message"] = match[3];
	    t = match.suffix();
	}
	while (boost::
	       regex_search(t, match, r_co,
			    boost::match_not_dot_newline)) {
	    header[match[1]] = match[2];
	    t = match.suffix();
	}
    }
    ofstream
	file_out;
    struct curl_slist *
	cheaders;
    string
	url,
	referel,
	cookie_file,
	proxy,
	proxy_usrpwd,
	post,
	usrpwd,
	user_agent,
	to_file;
    string
	headerP;
    char           *
	ebuff;
  public:
    // Return
    string page, error;
    map < string, string > header;
    map < string, string >::iterator h_it;
    // Constructor
    Curl() {
	// Load libcURL first
	if (dlsym(NULL, "curl_easy_init") == NULL) {
#ifdef WINDOWS
	    cURL_lib =
		dlopen("libcurl.dll",
		       RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE);
	    if (cURL_lib == NULL) {
		throw
		Curl_NoLoadLib();
	    }
#endif
#ifndef WINDOWS
	    int
		x;
	    for (x = 0; cURL_lib_names[x] != NULL; x++) {
		if ((cURL_lib =
		     dlopen((const char *) cURL_lib_names[x],
			    RTLD_NOW | RTLD_GLOBAL | RTLD_NODELETE)))
		    break;
	    }
	    if (cURL_lib_names[x] == NULL) {
		throw
		Curl_NoLoadLib();
	    }
#endif
	} else {
	    cURL_lib = NULL;
	}
	c_easy_init = (CURL * (*)())dlsym(cURL_lib, "curl_easy_init");
	c_easy_perform =
	    (CURLcode(*)(CURL *)) dlsym(cURL_lib, "curl_easy_perform");
	c_easy_setopt =
	    (CURLcode(*)(CURL *, CURLoption,...))dlsym(cURL_lib,
						       "curl_easy_setopt");
	c_easy_getinfo =
	    (CURLcode(*)(CURL *, CURLINFO,...))dlsym(cURL_lib,
						     "curl_easy_getinfo");
	c_easy_cleanup =
	    (void (*)(CURL *)) dlsym(cURL_lib, "curl_easy_cleanup");
	c_easy_escape =
	    (char *(*)(CURL *, char *, int)) dlsym(cURL_lib,
						   "curl_easy_escape");
	c_easy_unescape =
	    (char *(*)(CURL *, char *, int, int *)) dlsym(cURL_lib,
							  "curl_easy_unescape");
	c_free = (void (*)(void *)) dlsym(cURL_lib, "curl_free");
	c_formadd =
	    (CURLFORMcode(*)
	     (struct curl_httppost **,
	      struct curl_httppost **,...)) dlsym(cURL_lib,
						  "curl_formadd");
	c_formfree =
	    (void (*)(struct curl_httppost *)) dlsym(cURL_lib,
						     "curl_formfree");
	c_slist_append =
	    (struct curl_slist *
	     (*)(struct curl_slist *, const char *)) dlsym(cURL_lib,
							   "curl_slist_append");
	c_slist_free_all =
	    (void (*)(struct curl_slist *)) dlsym(cURL_lib,
						  "curl_slist_free_all");
	if (!(ch = (*c_easy_init) ())) {
	    throw
	    Curl_EasyInit();
	}
	ebuff = new char[1024];
	(*c_easy_setopt) (ch, CURLOPT_NOPROGRESS, TRUE);
	(*c_easy_setopt) (ch, CURLOPT_WRITEFUNCTION, &Curl::write_mem);
	(*c_easy_setopt) (ch, CURLOPT_WRITEDATA, (void *) this);
	(*c_easy_setopt) (ch, CURLOPT_HEADERFUNCTION, &Curl::write_header);
	(*c_easy_setopt) (ch, CURLOPT_WRITEHEADER, (void *) this);
	(*c_easy_setopt) (ch, CURLOPT_ERRORBUFFER, ebuff);
	(*c_easy_setopt) (ch, CURLOPT_SSL_VERIFYHOST, FALSE);
	(*c_easy_setopt) (ch, CURLOPT_SSL_VERIFYPEER, FALSE);
	(*c_easy_setopt) (ch, CURLOPT_FOLLOWLOCATION, TRUE);
	(*c_easy_setopt) (ch, CURLOPT_TIMEOUT, 60);
	// (*c_easy_setopt)(ch, CURLOPT_VERBOSE, TRUE);
	r_co.assign("^(.+?): (.+?)$");
	r_he.assign("^HTTP/(.+?) (.+?) (.+?)$");
	use_form = false;
	use_file = false;
	custom_header = false;
	is_debug = false;
	f_first = NULL;
	f_last = NULL;
    }
    const string
    fetch() {
	if (is_debug) {
	    cerr << "curl:fetch " << endl;
	}
	page.erase();
	headerP.erase();
	error.erase();
	if (use_form == true) {
	    (*c_easy_setopt) (ch, CURLOPT_POSTFIELDS, NULL);
	    (*c_easy_setopt) (ch, CURLOPT_POST, FALSE);
	    (*c_easy_setopt) (ch, CURLOPT_HTTPPOST, f_first);
	}
	if (custom_header) {
	    (*c_easy_setopt) (ch, CURLOPT_HTTPHEADER, cheaders);
	}
	if ((*c_easy_perform) (ch)) {
	    error.assign(ebuff);
	    this->set_post();
	    if (use_file)
		file_out.close();
	    return "ERROR";
	} else {
	    parse_header();
	    this->set_post();
	    if (use_file)
		file_out.close();
	    return page;
	}
    }
    string
    encode(string s) {
	if (is_debug) {
	    cerr << "curl:encode " + s << endl;
	}
	char           *
	    t = (*c_easy_escape) (ch, (char *) s.data(), s.size());
	string
	    ret = t;
	(*c_free) (t);
	return ret;
    }
    string
    decode(string s) {
	if (is_debug) {
	    cerr << "curl:decode " + s << endl;
	}
	int
	    l;
	char           *
	    t = (*c_easy_unescape) (ch, (char *) s.data(), s.size(), &l);
	string
	    ret = t;
	(*c_free) (t);
	return ret;
    }
    void
    add_header(string s, string c) {
	if (is_debug) {
	    cerr << "curl:add_header: " + s + " " + c << endl;
	}
	cheaders =
	    (*c_slist_append) (cheaders,
			       string(s + string(": ") + c).data());
	if (!custom_header) {
	    custom_header = true;
	}
    }
    void
    clear_header() {
	if (is_debug) {
	    cerr << "curl:clear_header" << endl;
	}
	if (custom_header) {
	    (*c_easy_setopt) (ch, CURLOPT_HTTPHEADER, NULL);
	    (*c_slist_free_all) (cheaders);
	    custom_header = false;
	}
    }
    void
    debug_on() {
	cerr << "curl:debug_on: Debug ON" << endl;
	(*c_easy_setopt) (ch, CURLOPT_VERBOSE, TRUE);
	is_debug = true;
    }
    void
    debug_off() {
	cerr << "curl:debug_off: Debug OFF" << endl;
	(*c_easy_setopt) (ch, CURLOPT_VERBOSE, FALSE);
	is_debug = false;
    }
    // Set Options
    void
    set_tofile(string name) {
	if (is_debug) {
	    cerr << "curl:set_tofile " + name << endl;
	}
	(*c_easy_setopt) (ch, CURLOPT_WRITEFUNCTION, &Curl::write_file);
	(*c_easy_setopt) (ch, CURLOPT_WRITEDATA, (void *) this);
	to_file = name;
	file_out.open(name.data(), ofstream::binary);
	use_file = true;
    }
    void
    set_tofile() {
	if (is_debug) {
	    cerr << "curl:set_tofile cleared" << endl;
	}
	(*c_easy_setopt) (ch, CURLOPT_WRITEFUNCTION, &Curl::write_mem);
	(*c_easy_setopt) (ch, CURLOPT_WRITEDATA, (void *) this);
	if (use_file)
	    file_out.close();
	use_file = false;
    }
    void
    set_url(string s) {
	if (is_debug) {
	    cerr << "curl:set_url " + s << endl;
	}
	if (s.length() > 0) {
	    (*c_easy_setopt) (ch, CURLOPT_URL, s.data());
	} else {
	    throw "Curl: No URL Specified";
	}
	url.assign(s);
    }
    void
    set_followlocation(bool b) {
	if (is_debug) {
	    cerr << "curl:set_followlocation " << b << endl;
	}
	if (b == true) {
	    (*c_easy_setopt) (ch, CURLOPT_FOLLOWLOCATION, TRUE);
	} else {
	    (*c_easy_setopt) (ch, CURLOPT_FOLLOWLOCATION, FALSE);
	}
    }
    void
    set_referel(string s) {
	if (is_debug) {
	    cerr << "curl:set_referel " + s << endl;
	}
	(*c_easy_setopt) (ch, CURLOPT_REFERER, s.data());
	referel.assign(s);
    }
    void
    set_referel() {
	if (is_debug) {
	    cerr << "curl:set_referel (no args)" << endl;
	}
	(*c_easy_setopt) (ch, CURLOPT_REFERER, NULL);
	referel.assign("");
    }
    void
    set_cookie_file(string s) {
	if (is_debug) {
	    cerr << "curl:set_cookie_file " + s << endl;
	}
	cookie_file.assign(s);
	// (*c_easy_setopt)(ch, CURLOPT_COOKIEJAR, cookie_file.data());
	(*c_easy_setopt) (ch, CURLOPT_COOKIEFILE, cookie_file.data());
    }
    void
    set_cookie_file() {
	if (is_debug) {
	    cerr << "curl:set_cookie_file (no args)" << endl;
	}
	(*c_easy_setopt) (ch, CURLOPT_COOKIEJAR, NULL);
	(*c_easy_setopt) (ch, CURLOPT_COOKIEFILE, NULL);
	cookie_file.assign("");
    }
    void
    set_proxy(string s, int type = CURLPROXY_SOCKS5) {
	if (is_debug) {
	    cerr << "curl:set_proxy " + s + " " << type << endl;
	}
	(*c_easy_setopt) (ch, CURLOPT_PROXY, s.data());
	(*c_easy_setopt) (ch, CURLOPT_PROXYTYPE, type);
	proxy.assign(s);
    }
    void
    set_proxy() {
	if (is_debug) {
	    cerr << "curl:set_proxy (no args)" << endl;
	}
	(*c_easy_setopt) (ch, CURLOPT_PROXY, NULL);
	proxy.assign("");
    }
    void
    set_proxy_usrpwd(string s) {
	(*c_easy_setopt) (ch, CURLOPT_PROXYUSERPWD, s.data());
	(*c_easy_setopt) (ch, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
	proxy_usrpwd.assign(s);
    }
    void
    set_proxy_usrpwd() {
	(*c_easy_setopt) (ch, CURLOPT_PROXYUSERPWD, NULL);
	proxy_usrpwd.assign("");
    }
    void
    set_post(string s) {
	if (is_debug) {
	    cerr << "curl:set_post " + s << endl;
	}
	if (use_form != true) {
	    if (s.length() > 0) {
		(*c_easy_setopt) (ch, CURLOPT_POST, TRUE);
		(*c_easy_setopt) (ch, CURLOPT_POSTFIELDS, s.data());
	    } else {
		(*c_easy_setopt) (ch, CURLOPT_POST, FALSE);
	    }
	    post.assign(s);
	}
    }
    void
    set_post_string(string n, string c) {
	if (is_debug) {
	    cerr << "curl:set_post_string " + n + " " + c << endl;
	}
	if (use_form != true)
	    use_form = true;
	(*c_formadd) (&f_first, &f_last, CURLFORM_COPYNAME, n.data(),
		      CURLFORM_COPYCONTENTS, c.data(), CURLFORM_END);
    }
    void
    set_post_file(string n, string f) {
	if (is_debug) {
	    cerr << "curl:set_post_file " + n + " " + f << endl;
	}
	if (use_form != true)
	    use_form = true;
	(*c_formadd) (&f_first, &f_last, CURLFORM_COPYNAME, n.data(),
		      CURLFORM_FILE, f.data(), CURLFORM_END);
    }
    void
    set_post_file_mem(string n, string f, const char *b, int len) {
	if (is_debug) {
	    cerr << "curl:set_post_file_mem " + n + " " + f << endl;
	}
	if (use_form != true)
	    use_form = true;
	(*c_formadd) (&f_first, &f_last, CURLFORM_COPYNAME, n.data(),
		      CURLFORM_BUFFER, f.data(), CURLFORM_BUFFERPTR, b,
		      CURLFORM_BUFFERLENGTH, len, CURLFORM_END);
    }
    void
    set_post() {
	if (is_debug) {
	    cerr << "curl:set_post (no args)" << endl;
	}
	if (f_first != NULL) {
	    (*c_formfree) (f_first);
	    use_form = false;
	    f_first = NULL;
	    f_last = NULL;
	}
	(*c_easy_setopt) (ch, CURLOPT_POSTFIELDS, NULL);
	(*c_easy_setopt) (ch, CURLOPT_HTTPPOST, NULL);
	(*c_easy_setopt) (ch, CURLOPT_POST, FALSE);
	post.assign("");
    }
    void
    set_userpwd(string s) {
	if (is_debug) {
	    cerr << "curl:set_userpwd " + s << endl;
	}
	(*c_easy_setopt) (ch, CURLOPT_USERPWD, s.data());
	usrpwd.assign(s);
    }
    void
    set_userpwd() {
	if (is_debug) {
	    cerr << "curl:set_userpwd (no args)" << endl;
	}
	(*c_easy_setopt) (ch, CURLOPT_USERPWD, NULL);
	usrpwd.assign("");
    }
    void
    set_agent(string s) {
	if (is_debug) {
	    cerr << "curl:set_agent " + s << endl;
	}
	(*c_easy_setopt) (ch, CURLOPT_USERAGENT, s.data());
	user_agent.assign(s);
    }
    void
    set_agent() {
	if (is_debug) {
	    cerr << "curl:set_agent (no args)" << endl;
	}
	(*c_easy_setopt) (ch, CURLOPT_USERAGENT, NULL);
	user_agent.assign("");
    }
    // Destructor
    ~Curl() {
	(*c_easy_cleanup) (ch);
	if (cURL_lib)
	    dlclose(cURL_lib);
	delete[]ebuff;
    }
};
#endif
