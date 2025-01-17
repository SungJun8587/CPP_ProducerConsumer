
//***************************************************************************
// DBAsyncHandler.cpp : implementation of the DBAsyncHandler class.
//
//***************************************************************************

#include "pch.h"
#include "DBAsyncHandler.h"

DECLARE_DBASYNC_HANDLER(DBASYNC_BULKADD_CONSUMER_REQ)
{
	int32	iName1Size = 50;
	int32	iName2Size = 50;

	CONSUMER_DATA_BATCH_REQ* pDBParam = (CONSUMER_DATA_BATCH_REQ*)pStAsync;

	auto pOdbcConn = COdbcAsyncSrv::Instance()->GetAccountOdbcConnPool()->GetOdbcConn(gpThreadManager->getThreadID() - 1);
	if( nullptr == pOdbcConn )
		return EDBReturnType::INVALID;

	const uint64 rowCount = BATCH_SIZE;  // ������ ���� ��

	// �ڵ� Ŀ�� ��� Off ����
	pOdbcConn->SetAutoCommitMode(SQL_AUTOCOMMIT_OFF);

	pOdbcConn->InitStmtHandle();

	// ����� Ŀ���� ������ ����(���� Ŀ���� Ȱ��ȭ)
	pOdbcConn->SetStmtAttr(SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_DYNAMIC, 0);

	// Ŀ���� ���ü�(concurrency) ���� ����� ����(�� ���� ��� ���ü� ����)
	pOdbcConn->SetStmtAttr(SQL_ATTR_CONCURRENCY, (SQLPOINTER)SQL_CONCUR_ROWVER, 0);

	// �� ���� ���ε��� ���� ��� ���� ���ε��� ������ ���� �Ǵ� ������ ���̷� ����
	pOdbcConn->SetStmtAttr(SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)sizeof(CONSUMER_DATA), SQL_IS_UINTEGER);

	// �뷮 �۾� �� ũ�� ����(�� ���� ������ �� ��)
	pOdbcConn->SetStmtAttr(SQL_ATTR_ROW_ARRAY_SIZE, reinterpret_cast<SQLPOINTER>(rowCount), 0);

	if( !pOdbcConn->ExecDirect(_T("SELECT Name1, Name2, Flag, Age FROM Consumer WHERE 1=0")) )
		return EDBReturnType::INVALID;

	pOdbcConn->BindCol(pDBParam->_consumers[0].tszName1, iName1Size);
	pOdbcConn->BindCol(pDBParam->_consumers[0].tszName2, iName2Size);
	pOdbcConn->BindCol(pDBParam->_consumers[0].bFlag);
	pOdbcConn->BindCol(pDBParam->_consumers[0].nAge);

	if( !pOdbcConn->BulkOperations(SQL_ADD) )
		return EDBReturnType::INVALID;

	pOdbcConn->Commit();

	pOdbcConn->UnBindColStmt();

	return EDBReturnType::OK;
}

