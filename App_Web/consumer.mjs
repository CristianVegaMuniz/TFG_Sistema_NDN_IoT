import { Data, Name } from "@ndn/packet";
import { fetch } from "@ndn/segmented-object";
import { NameTree, NameTreeFetcher } from "@ndn/naming-convention2";

async function consumer() {
  const prefix = new Name("/temp");
  const fetcher = new NameTreeFetcher({
    prefix: prefix,
    conventions: NameTree.Conventions.B, // use binary trie convention
  });
  
  while (true) {
    const [data] = await fetch(fetcher, { retries: 3 });
    if (data) {
      console.log(`Data received: ${data.name}`);
    } else {
      console.log(`No data received`);
    }
    await new Promise((resolve) => setTimeout(resolve, 5000)); // wait for 5 seconds
  }
}

module.exports = consumer;
