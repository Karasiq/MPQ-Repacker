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
        int m_fileIdx;    // ������ ������ ����� ����� �����

        bool m_isServerPath;
        bool m_canBeFile; // ���� ����� �������� ���� �� �����. �.�., � ����� ���� ��� �����

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

    // ��������� �� ������������ ��� �����

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

        // �������� �� ������ ����������� ��� ������� � ���������� ��������

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

        // ����� �������� �������� �� ������������

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

    // ��������� �� ������������ ���� �� ����� ��� �����

    template< typename T >
    static bool testPath_T( const T* pFilePath, PathResult& res )
    {
        res.m_code = UNKNOWN;
        res.m_errPos = -1;
        res.m_lastDot = -1;
        res.m_fileIdx = -1;
        res.m_isServerPath = false;
        res.m_canBeFile = false;

        // ������ ���� ������ ����, �� ������� ���� 2 �������.
        // �������� "C:" ��� "\\"
        if( !pFilePath || !pFilePath[0] || !pFilePath[1] )
        {
            res.m_code = INVALID_LENGTH;
            res.m_errPos = 0;
            return false;
        }

        int start = 2;

        // �������� ����� �����

        res.m_isServerPath = pFilePath[0] == T('\\') && pFilePath[1] == T('\\');

        if( res.m_isServerPath )
        {
            // �� ����� ���� ��� ����� ������
            if( isSlash( pFilePath[2] ) )
            {
                res.m_code = INVALID_CHARACTER;
                res.m_errPos = 2;
                return false;
            }
        }
        else
        {
            // ������ ������ ������ ���� ":"
            if( T(':') != pFilePath[1] )
            {
                res.m_code = INVALID_CHARACTER;
                res.m_errPos = 1;
                return false;
            }
            
            // ������ ������ ������ ���� ����������� ������
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

        // ������ "start" ��������� �� ����� ��������� �� ������������ �����
        // �������� ��� "C:\some" start == 3, ��� "\\Some" start == 2.

        int beginIdx = start; // ������ ����� ����� ���������� �����

        NameResult fileRes;

        int i = start; // ��������

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

        // ���� ��� ����� ������� �������
        if( i > MAX_PATH )
        {
            res.m_code = INVALID_LENGTH;
            res.m_errPos = i;
            
            return false;
        }

        // ���� � ����� �� ���� ������������ �����
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