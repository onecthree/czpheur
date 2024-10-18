#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include <Zend/zend_smart_str.h>
#include <Zend/zend_smart_string.h>
#include <ext/standard/html.h>

#include <include/regexp.h>
#include <include/onecstr.h>
#include "zerror.h"

zend_string* trim_error_message( char const* errstr )
{
	// size_t      dest_size = 2;
	// size_t      dest_index = 0;
	// char*       dest_buffer = malloc(2);
	onec_string*   error_message;
	onec_string_init(error_message);

	size_t      error_index;
	size_t      error_char;

	size_t      index_point = 0;
	bool        prefix = false;
	int         postfix = 4;

	while( (error_char = errstr[error_index]) != '\0' )
	{
		++error_index;

		if( !prefix )
		{
			if( error_char == ':' )
			{
				++error_index; // Skip early (begin) whitespace
				prefix = true;
				continue;
			}

			continue;
		}

		switch( error_char )
		{
			case 'i':
			case 'n':
			case ' ':
			case '/':
				postfix -= 1;
			break;
			default:
				postfix = 4;
			break;
		}

		if( !postfix )
		{
			// error_message->len -= 3;
			break;
		}

		onec_string_put(error_message, error_char);

		// if( dest_index >= dest_size )
		// {
		// 	dest_size *= 2;
		// 	dest_buffer = realloc(dest_buffer, dest_size);
		// }

		// dest_buffer[dest_index] = error_char;
		// ++dest_index;
	}

	// zend_string *retval = zend_string_init(dest_buffer, index_point - 1, 0);
	zend_string* retval = zend_string_init(error_message->val, error_message->len, 0);

	return retval;
}

void zpheur_errorhandler( int errtype, zend_string *errfile, const uint32_t errline, zend_string *errstr )
{
	FILE *errfile_ptr = fopen(errfile->val, "r");

	smart_str sourcecode_buffer = {0};
	smart_str lineno_buffer = {0};

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    size_t errcode_current_line = 0;
    size_t errcode_min_line = 1;

    // set target break after 2 line
    size_t errcode_end_line = 3;

    if( errline > 8 )
    {
    	errcode_min_line = errline - 7;
    }

	while( (read = getline(&line, &len, errfile_ptr)) != -1 )
	{
		++errcode_current_line;

		if( errcode_current_line < errcode_min_line )
		{
			continue;
		}

		if( errcode_current_line > errline )
		{
			--errcode_end_line;
		}

		if( !errcode_end_line ) // when 0
		{
			break;
		}

		zend_string *_entitied_line = php_escape_html_entities_ex(
		line, len, 1, ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401,
		NULL, true, 0);

		if( errline == errcode_current_line )
		{
			size_t _lineno_len = snprintf(NULL, 0, "<div class=\"highlight-line\">&nbsp;%ld</div>", errcode_current_line);
			char _lineno_buffer[_lineno_len];
			php_sprintf(_lineno_buffer, "<div class=\"highlight-line\">&nbsp;%ld</div>", errcode_current_line);

			smart_str_appends(&lineno_buffer, _lineno_buffer);

			size_t _sourcecode_len = snprintf(NULL, 0, "<div class=\"highlight-code\">%s</div>", _entitied_line->val);
			char _sourcecode_buffer[_sourcecode_len];
			php_sprintf(_sourcecode_buffer, "<div class=\"highlight-code\">%s</div>", _entitied_line->val);

			smart_str_appends(&sourcecode_buffer, _sourcecode_buffer);
		}
		else
		{
			size_t _lineno_len = snprintf(NULL, 0, "<div>&nbsp;%ld</div>", errcode_current_line);
			char _lineno_buffer[_lineno_len];
			php_sprintf(_lineno_buffer, "<div>&nbsp;%ld</div>", errcode_current_line);

			smart_str_appends(&lineno_buffer, _lineno_buffer);

			size_t _sourcecode_len = snprintf(NULL, 0, "<div>%s</div>", _entitied_line->val);
			char _sourcecode_buffer[_sourcecode_len];
			php_sprintf(_sourcecode_buffer, "<div>%s</div>", _entitied_line->val);

			smart_str_appends(&sourcecode_buffer, _sourcecode_buffer);
		}
	}

	smart_str_0(&sourcecode_buffer);
	smart_str_0(&lineno_buffer);

	char const *short_errstr;

	switch(errtype)
	{
	case E_WARNING:
		short_errstr = "Warning";
	break;
	case E_ERROR:
		short_errstr = "Error";
	break;
 	case E_FATAL_ERRORS:
 		short_errstr = "FatalErrors";
 	break;
 	case E_PARSE:
 		short_errstr = "Parse";
 	break;
 	case E_COMPILE_WARNING:
 		short_errstr = "CompileWarning";
 	break;
 	case E_COMPILE_ERROR:
 		short_errstr = "CompileError";
 	break;
	default:
		short_errstr = "UncaughtError";
		// errstr = trim_error_message(errstr->val);
		// errstr = errstr;
	break;
	}

	// Only once call when first error trigger
	if( zend_get_constant_str("ZPHEUR_ERROR_ALREADY_THROW", sizeof("ZPHEUR_ERROR_ALREADY_THROW") - 1) == NULL )
	{
		zend_register_long_constant("ZPHEUR_ERROR_ALREADY_THROW", sizeof("ZPHEUR_ERROR_ALREADY_THROW") - 1, 0, CONST_CS | CONST_PERSISTENT, 0);

		// clean previous buffer
		if( OG(active) )
		{
			php_output_clean();
		}

		php_printf(R"(
<!DOCTYPE html>
<html lang=en>
<head>
<meta charset=UTF-8>
<meta name=viewport content="width=device-width, initial-scale=1.0">
<title>PHP Error Exception</title>
</head>
<style>body{background:radial-gradient(#712B75 5%%,#16213E 95%%);background-repeat:no-repeat;background-attachment:fixed}#container{width:1200px;margin:0 auto}.frame-error-info{margin-top:20px}.sourcecode-frame{font-size:17px;font-family:Menlo,Monaco,Consolas,"Courier New",monospace;display:flex;background:#22092c;color:#f5e8c7;line-height:23px;padding-top:10px;padding-bottom:10px;border-radius:0 0 7px 7px}.sourcecode-tab{display:inline-flex;width:100%%;background:#0a2647;overflow:hidden;cursor:pointer}.sourcecode-tab .title{color:#eee;background:#22092c;overflow:hidden;padding:10px;font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Helvetica,Arial,sans-serif,"Apple Color Emoji","Segoe UI Emoji";white-space:nowrap;border-left:3px solid #000}.sourcecode-tab .title-back{background:#31304d}.lines{padding-right:4px;font-weight:bold;text-align:right;padding-top:30px;padding-bottom:30px;border:0 solid red;border-width:0 0 0 4px;background:#232d3f}.lines div{padding-right:4px}pre{min-height:100px;background:red}.codes{white-space:pre;display:block;unicode-bidi:embed;padding-top:30px;padding-bottom:10px;overflow:hidden}.codes div{padding-left:10px;display:block;width:100vw}.header-error-info{font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Helvetica,Arial,sans-serif,"Apple Color Emoji","Segoe UI Emoji";height:content-fit;background:#0a2647;border:0 solid #4e31aa;border-width:10px 0 0 0;border-radius:7px 7px 0 0;padding:10px}.header-error-info .title{display:flex;font-size:23px;color:#eee;vertical-align:middle}.header-error-info .subheader-error-info{margin-top:50px;padding:4px;font-size:23px;color:#eee;height:content-fit;background:#31304d;border:2px solid #31304d;border-radius:4px}.header-error-info .file{opacity:.7;margin-top:3px;font-size:17px}.highlight-line{width:100%%;background:#313866}.highlight-code{background:#3f1d38}</style>
<script>let globalErrorStacks=[];let tabErrorFiles=[];function switchErrorContext(errorId)
{document.querySelectorAll("#container")[0].innerHTML=`<div class="frame-error-info"><div class="header-error-info"><div class="title"><svg xmlns="http://www.w3.org/2000/svg"width="24"height="24"fill="red"class="bi bi-exclamation-triangle-fill"viewBox="0 0 16 16"><path d="M8.982 1.566a1.13 1.13 0 0 0-1.96 0L.165 13.233c-.457.778.091 1.767.98 1.767h13.713c.889 0 1.438-.99.98-1.767L8.982 1.566zM8 5c.535 0 .954.462.9.995l-.35 3.507a.552.552 0 0 1-1.1 0L7.1 5.995A.905.905 0 0 1 8 5m.002 6a1 1 0 1 1 0 2 1 1 0 0 1 0-2"/></svg>&nbsp;${globalErrorStacks[errorId].title}</div><div class="subheader-error-info"><div>${globalErrorStacks[errorId].message}</div><div class="file">${globalErrorStacks[errorId].file}</div></div></div><div class="sourcecode-tab">${tabErrorFiles.map((el,ind)=>ind==errorId?!(el.classList.remove("title-back"))?el.outerHTML:0:(el.innerHTML=globalErrorStacks[ind].message)&&!el.classList.add("title-back")?el.outerHTML:0).join``}</div><div class="sourcecode-frame"><div class="lines">${globalErrorStacks[errorId].lines}</div><div class="codes">${globalErrorStacks[errorId].codes}</div></div>`;}
document.addEventListener("DOMContentLoaded",function(event){let errorStacks=document.querySelectorAll(".frame-error-info");errorStacks.forEach((error,index)=>{let newDiv=document.createElement("div");newDiv.classList.add("title");newDiv.setAttribute("onclick",`javascript:switchErrorContext(${index});`);newDiv.innerHTML=error.querySelectorAll(".header-error-info .subheader-error-info .message")[0].innerHTML;tabErrorFiles.push(newDiv);globalErrorStacks.push({title:error.querySelectorAll(".header-error-info .title")[0].innerHTML.match(/(?<=\&nbsp\;).*$/),message:error.querySelectorAll(".header-error-info .subheader-error-info .message")[0].innerHTML,file:error.querySelectorAll(".header-error-info .subheader-error-info .file")[0].innerHTML,lines:error.querySelectorAll(".lines")[0].innerHTML,codes:error.querySelectorAll(".codes")[0].innerHTML,});});document.querySelectorAll("#container")[0].innerHTML=`<div class="frame-error-info"><div class="header-error-info"><div class="title"><svg xmlns="http://www.w3.org/2000/svg"width="24"height="24"fill="red"class="bi bi-exclamation-triangle-fill"viewBox="0 0 16 16"><path d="M8.982 1.566a1.13 1.13 0 0 0-1.96 0L.165 13.233c-.457.778.091 1.767.98 1.767h13.713c.889 0 1.438-.99.98-1.767L8.982 1.566zM8 5c.535 0 .954.462.9.995l-.35 3.507a.552.552 0 0 1-1.1 0L7.1 5.995A.905.905 0 0 1 8 5m.002 6a1 1 0 1 1 0 2 1 1 0 0 1 0-2"/></svg>&nbsp;${globalErrorStacks[0].title}</div><div class="subheader-error-info"><div>${globalErrorStacks[0].message}</div><div class="file">${globalErrorStacks[0].file}</div></div></div><div class="sourcecode-tab">${tabErrorFiles.map((el,ind)=>ind>0?!el.classList.add("title-back")?el.outerHTML:0:el.outerHTML).join``}</div><div class="sourcecode-frame"><div class="lines">${globalErrorStacks[0].lines}</div><div class="codes">${globalErrorStacks[0].codes}</div></div>`;});</script>
<body>
<div id=container>
    )");

	}

// 	php_printf(R"(
//         <div class="block-top">
//         <div class="header-report">
//             <div class="title">PHP::%s</div>
//             <div class="report-title">%s</div>
//         </div>
//         <div class="tab-code">
//             <div class="title">%s — at line %d</div>
//         </div>
//         <div class="main">
//             <div class="line-counter">%s</div>
//             <div class="main-code">%s</div>
//         </div>    
//         </div>
// )", short_errstr, errstr->val, errfile->val, errline,
// 	smart_str_extract(&lineno_buffer)->val,
// 	smart_str_extract(&sourcecode_buffer)->val);


	// Error throw per iteration
	php_printf(R"(
        <div class="frame-error-info">
        <div class="header-error-info">
            <div class="title">
            <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" fill="red" class="bi bi-exclamation-triangle-fill" viewBox="0 0 16 16">
  <path d="M8.982 1.566a1.13 1.13 0 0 0-1.96 0L.165 13.233c-.457.778.091 1.767.98 1.767h13.713c.889 0 1.438-.99.98-1.767L8.982 1.566zM8 5c.535 0 .954.462.9.995l-.35 3.507a.552.552 0 0 1-1.1 0L7.1 5.995A.905.905 0 0 1 8 5m.002 6a1 1 0 1 1 0 2 1 1 0 0 1 0-2"/>
</svg>&nbsp;PHP::%s</div>
            <div class="subheader-error-info">
                <div class="message">%s</div>
                <div class="file">in %s — at line %d</div>
            </div>
        </div>
        <div class="sourcecode-tab">
            <div class="title">%s</div>
        </div>
        <div class="sourcecode-frame">
            <div class="lines">%s</div>
            <div class="codes">%s</div>
	)", short_errstr, errstr->val, errfile->val, errline, errstr->val,
	smart_str_extract(&lineno_buffer)->val,
	smart_str_extract(&sourcecode_buffer)->val);

}