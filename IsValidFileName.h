#ifndef __ISVALIDFILENAME_H__
#define __ISVALIDFILENAME_H__

class FileName
{
public:
    enum RES_CODE
    {
        OK = 0,

        UNKNOWN,

        INVALID_LENGTH,

        FILENAME_CLOCK,
        FILENAME_AUX,
        FILENAME_CON,
        FILENAME_NUL,
        FILENAME_PRN,
        FILENAME_COM1,
        FILENAME_COM2,
        FILENAME_COM3,
        FILENAME_COM4,
        FILENAME_COM5,
        FILENAME_COM6,
        FILENAME_COM7,
        FILENAME_COM8,
        FILENAME_COM9,
        FILENAME_LPT1,
        FILENAME_LPT2,
        FILENAME_LPT3,
        FILENAME_LPT4,
        FILENAME_LPT5,
        FILENAME_LPT6,
        FILENAME_LPT7,
        FILENAME_LPT8,
        FILENAME_LPT9,

        INVALID_CHARACTER,
    };

    class NameResult
    {
        friend class FileName;

        RES_CODE m_code;
        int m_errPos;
        int m_lastDot;

    public:
        NameResult() : m_code(UNKNOWN), m_errPos(-1), m_lastDot(-1)
        {}

        RES_CODE code() const { return m_code; }
        int errPos() const { return m_errPos; }
        int lastDot() const { return m_lastDot; }

        bool isOk() const { return OK == m_code; }
        bool isDeviceName() const { return m_code >= FILENAME_CLOCK && m_code <= FILENAME_LPT9; }
    };

    class PathResult
    {
        friend class FileName;

        RES_CODE m_code;
        int m_errPos;
        int m_lastDot;
        int m_fileIdx;    // индекс первой буквы имени файла

        bool m_isServerPath;
        bool m_canBeFile; // путь может оказатся путём до файла. Т.е., в конце пути нет слеша

    public:
        PathResult()
        : m_code(UNKNOWN)
        , m_errPos(-1)
        , m_lastDot(-1)
        , m_fileIdx(-1)
        , m_isServerPath(false)
        , m_canBeFile(false)
        {}

        RES_CODE code() const { return m_code; }
        int errPos() const { return m_errPos; }
        int lastDot() const { return m_lastDot; }
        int fileIdx() const { return m_fileIdx; }
        bool isServerPath() const { return m_isServerPath; }
        bool canBeFile() const { return m_canBeFile; }

        bool isOk() const { return OK == m_code; }
        bool isDeviceName() const { return m_code >= FILENAME_CLOCK && m_code <= FILENAME_LPT9; }
    };

private:
    static RES_CODE IsStdDeviceName( const char* pFileName, int len );
    static RES_CODE IsStdDeviceName( const wchar_t* pFileName, int len );

    template< typename T >
    static bool isSlash( T ch ) { return ch == T('\\') || ch == T('/'); }

    // Проверяет на правильность имя файла

    template< typename T >
    static bool testName_T( const T* pFileName, int len, NameResult& res )
    {
        res.m_code = UNKNOWN;
        res.m_errPos = -1;
        res.m_lastDot = -1;

        if( !pFileName || !*pFileName || 0 == len || len > 255 )
        {
            res.m_code = INVALID_LENGTH;
            res.m_errPos = 0;
            return false;
        }

        RES_CODE deviceCode = IsStdDeviceName( pFileName, len );
        
        if( OK != deviceCode )
        {
            res.m_code = deviceCode;
            res.m_errPos = 0;
            return false;
        }

        // Проверка на особые ограничения для первого и последнего символов

        switch( pFileName[0] )
        {
        case T(' '):
            res.m_code = INVALID_CHARACTER;
            res.m_errPos = 0;
            return false;
        }

        switch( pFileName[len-1] )
        {
        case T('.'):
        case T(' '):
            res.m_code = INVALID_CHARACTER;
            res.m_errPos = len-1;
            return false;
        }

        // Общая проверка символов на правильность

        for( int i = 0; i < len; i++ )
        {
            switch( pFileName[i] )
            {
            case T('\\'):
            case T('/'):
            case T('|'):
            case T(':'):
            case T('*'):
            case T('?'):
            case T('>'):
            case T('<'):
            case T('\"'):
            case T('\t'):
                res.m_code = INVALID_CHARACTER;
                res.m_errPos = i;
                return false;

            case '.':
                res.m_lastDot = i;
                break;
            }
        }

        res.m_code = OK;
        return true;
    }

    // Проверяет на правильность путь до файла или папки

    template< typename T >
    static bool testPath_T( const T* pFilePath, PathResult& res )
    {
        res.m_code = UNKNOWN;
        res.m_errPos = -1;
        res.m_lastDot = -1;
        res.m_fileIdx = -1;
        res.m_isServerPath = false;
        res.m_canBeFile = false;

        // Длинна пути должна быть, по крайней мере 2 символа.
        // например "C:" или "\\"
        if( !pFilePath || !pFilePath[0] || !pFilePath[1] )
        {
            res.m_code = INVALID_LENGTH;
            res.m_errPos = 0;
            return false;
        }

        int start = 2;

        // Проверка буквы диска

        res.m_isServerPath = pFilePath[0] == T('\\') && pFilePath[1] == T('\\');

        if( res.m_isServerPath )
        {
            // не может быть три слеша подряд
            if( isSlash( pFilePath[2] ) )
            {
                res.m_code = INVALID_CHARACTER;
                res.m_errPos = 2;
                return false;
            }
        }
        else
        {
            // второй символ должен быть ":"
            if( T(':') != pFilePath[1] )
            {
                res.m_code = INVALID_CHARACTER;
                res.m_errPos = 1;
                return false;
            }
            
            // третий символ должен быть завершением слешем
            if( T('0') != pFilePath[2] )
            {
                if( !isSlash( pFilePath[2] ) )
                {
                    res.m_code = INVALID_CHARACTER;
                    res.m_errPos = 2;
                    return false;
                }

                start = 3;
            }
        }

        // Теперь "start" указывает на букву следующую за определением диска
        // например для "C:\some" start == 3, для "\\Some" start == 2.

        int beginIdx = start; // индекс буквы после последнего слеша

        NameResult fileRes;

        int i = start; // итератор

        for( ; pFilePath[i]; i++ )
        {
            if( isSlash( pFilePath[i] ) )
            {
                int len = i - beginIdx;

                if( 0 == len )
                {
                    res.m_code = INVALID_CHARACTER;
                    res.m_errPos = i;
                    return false;
                }

                if( !testName_T( &pFilePath[beginIdx], len, fileRes ) )
                {
                    res.m_code = fileRes.m_code;
                    res.m_errPos = beginIdx + fileRes.m_errPos;
                    return false;
                }

                beginIdx = i+1;
            }
        }

        // Если имя файла слишком длинное
        if( i > MAX_PATH )
        {
            res.m_code = INVALID_LENGTH;
            res.m_errPos = i;
            
            return false;
        }

        // Если в конце не было завершающего слеша
        if( T('\0') != pFilePath[beginIdx] )
        {
            res.m_canBeFile = true;
            res.m_fileIdx = beginIdx;

            int len = i - beginIdx - 1;

            if( !testName_T( &pFilePath[beginIdx], len, fileRes ) )
            {
                res.m_code = fileRes.m_code;
                res.m_errPos = beginIdx + fileRes.m_errPos;
                return false;
            }

            res.m_lastDot = fileRes.m_lastDot != -1 ? beginIdx + fileRes.m_lastDot : -1;
        }

        res.m_code = OK;
        return true;
    }

public:
    static bool testName( const char* pFileName, NameResult& res )
    {
        return testName_T( pFileName, (int)strlen( pFileName ), res );
    }

    static bool testName( const wchar_t* pFileName, NameResult& res )
    {
        return testName_T( pFileName, (int)wcslen( pFileName ), res );
    }

    static bool testPath( const char* pFilePath, PathResult& res )
    {
        return testPath_T( pFilePath, res );
    }

    static bool testPath( const wchar_t* pFilePath, PathResult& res )
    {
        return testPath_T( pFilePath, res );
    }

    static bool testName( const char* pFileName )
    {
        NameResult res; 
        return testName_T( pFileName, (int)strlen( pFileName ), res );
    }

    static bool testName( const wchar_t* pFileName )
    {
        NameResult res;
        return testName_T( pFileName, (int)wcslen( pFileName ), res );
    }

    static bool testPath( const char* pFilePath )
    {
        PathResult res;
        return testPath_T( pFilePath, res );
    }

    static bool testPath( const wchar_t* pFilePath )
    {
        PathResult res;
        return testPath_T( pFilePath, res );
    }
};
#endif