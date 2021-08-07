# python scraper to fetch most up-to-date active cases

import requests
from bs4 import BeautifulSoup
import pandas as pd

website = 'https://www.worldometers.info/coronavirus/#countries'  # url for the site
website_url = requests.get(website).text
soup = BeautifulSoup(website_url, 'html.parser')

my_table = soup.find('tbody')

table_data = []
for row in my_table.findAll('tr'):
    row_data = []
    for cell in row.findAll('td'):
        row_data.append(cell.text)
    if len(row_data) > 0:
        data_item = {"Country": row_data[1],
                     "ActiveCases": row_data[8],
                     }
        table_data.append(data_item)

df = pd.DataFrame(table_data)
df = df.drop(range(0, 8))
df.to_csv('active_cases.csv', index=False)
